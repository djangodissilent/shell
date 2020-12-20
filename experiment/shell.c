#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

//Regular text
#define BLK "\e[0;30m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"
#define BLU "\e[0;34m"
#define MAG "\e[0;35m"
#define CYN "\e[0;36m"
#define WHT "\e[0;37m"

//Regular bold text
#define BBLK "\e[1;30m"
#define BRED "\e[1;31m"
#define BGRN "\e[1;32m"
#define BYEL "\e[1;33m"
#define BBLU "\e[1;34m"
#define BMAG "\e[1;35m"
#define BCYN "\e[1;36m"
#define BWHT "\e[1;37m"

//Regular underline text
#define UBLK "\e[4;30m"
#define URED "\e[4;31m"
#define UGRN "\e[4;32m"
#define UYEL "\e[4;33m"
#define UBLU "\e[4;34m"
#define UMAG "\e[4;35m"
#define UCYN "\e[4;36m"
#define UWHT "\e[4;37m"

//Regular background
#define BLKB "\e[40m"
#define REDB "\e[41m"
#define GRNB "\e[42m"
#define YELB "\e[43m"
#define BLUB "\e[44m"
#define MAGB "\e[45m"
#define CYNB "\e[46m"
#define WHTB "\e[47m"

//High intensty background
#define BLKHB "\e[0;100m"
#define REDHB "\e[0;101m"
#define GRNHB "\e[0;102m"
#define YELHB "\e[0;103m"
#define BLUHB "\e[0;104m"
#define MAGHB "\e[0;105m"
#define CYNHB "\e[0;106m"
#define WHTHB "\e[0;107m"

//High intensty text
#define HBLK "\e[0;90m"
#define HRED "\e[0;91m"
#define HGRN "\e[0;92m"
#define HYEL "\e[0;93m"
#define HBLU "\e[0;94m"
#define HMAG "\e[0;95m"
#define HCYN "\e[0;96m"
#define HWHT "\e[0;97m"

//Bold high intensity text
#define BHBLK "\e[1;90m"
#define BHRED "\e[1;91m"
#define BHGRN "\e[1;92m"
#define BHYEL "\e[1;93m"
#define BHBLU "\e[1;94m"
#define BHMAG "\e[1;95m"
#define BHCYN "\e[1;96m"
#define BHWHT "\e[1;97m"

//Reset
#define reset "\e[0m"

// #include "h.h"

//move to header file
#define MAX_LINE 80 /* The maximum length command */

int _BG = 0;

//char *routines to parse and print the argument vector
char *parse_input(char *in, char *argv[MAX_LINE], char *args[MAX_LINE], int REDIRECT_FLAGS[]);
void print_argv(char *argv[MAX_LINE]);
// void sarrcpy(char *last[MAX_LINE], char *argv[]);
char *checkRedirect(int REDIRECT_FLAGS[], char *in, char *args[]);
char **toArgv(char *in, char *args[MAX_LINE]);
void prompt();
int main(void)
{
    //redirecr Flags
    int REDIRECT_FLAGS[3];
    memset(REDIRECT_FLAGS, 0, sizeof REDIRECT_FLAGS);
    //input buffer
    char in[MAX_LINE];
    char *argv[MAX_LINE] = {0};
    char *args[MAX_LINE] = {0};
    char *last[MAX_LINE] = {0};

    int __RUN = 1;
    while (__RUN)
    {
        //NO REDIRECT default
        memset(REDIRECT_FLAGS, 0, sizeof REDIRECT_FLAGS);

        //  H for HAPDD
        prompt();
        fflush(stdout);
        char *rFile = parse_input(in, argv, args, REDIRECT_FLAGS);
        // if change directory command
        if (!strcmp("cd", argv[0]))
        {
            chdir(argv[1]);
            continue;
        }
        pid_t pid = fork();

        //fork failed!
        if (pid < 0)
        {
            fprintf(stderr, "Failed Fork");
            return 1;
        }

        else if (pid == 0)
        {
            // REDIRECT IN
            if (REDIRECT_FLAGS[0])
            {
                int in;
                in = open(rFile, O_RDONLY);
                dup2(in, 0);
                close(in);
            }
            // REDIRECT OUT
            else if (REDIRECT_FLAGS[1])
            {
                int out;
                // printf("\n\n\n\n%s\n\n\n\n", rFile);
                out = open(rFile, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                // replace standard output with output file
                dup2(out, 1);
                close(out);
            }

            else if (REDIRECT_FLAGS[2])
            {
                int fd[2];
                pid_t pid2;
                if (pipe(fd) == -1)
                {
                    fprintf(stderr, "Pipe Failed");
                    return 1;
                }
                pid2 = fork();
                if (pid2 < 0)
                {
                    fprintf(stderr, "Fork Failed");
                    return 1;
                }
                if (pid2 > 0)
                {
                    // 1st child is executing
                    /* close the unused end of the pipe */
                    close(fd[0]);
                    dup2(fd[1], 1);
                    execvp(argv[0], argv);
                    /* write to the pipe */
                    /* close the write end of the pipe */
                    close(fd[1]);
                }
                else if (pid2 == 0)
                {
                    close(fd[1]);
                    dup2(fd[0], 0);
                    close(fd[0]);
                    // printf("#######%s####", rFile);
                    execvp(args[0], args);
                }
            }
            if (!REDIRECT_FLAGS[2])
            {

                //this commad traverses $PATH for argv[0]
                //and provides subsequent flags
                // print_argv(argv);
                // printf("%d\n", REDIRECT_FLAGS[1]);

                int ret = execvp(argv[0], argv); // Run argv[0] with argv[1..] flagss
                //set foreground
                //failed to find user command
                if (ret == -1)
                    fprintf(stderr, "%s: command not found\n", argv[0]);
            }
        }
        //root is here
        else
        {
            // wait for children
            if (REDIRECT_FLAGS[2])
            {
                wait(NULL);
                wait(NULL);
            }
            // wait for child
            else if (!_BG && !REDIRECT_FLAGS[2])
                wait(NULL);
            else if (_BG)
            {
                printf("\nExecuting %d Concurrently\n", pid);
            }
            continue;
        }
    }
    return 0;
}

char *parse_input(char *in, char *argv[MAX_LINE], char *args[MAX_LINE], int REDIRECT_FLAGS[3])
{
    //strtok modifies stirng
    char *tmp[MAX_LINE];

    scanf("%80[^\n]", in);
    getchar();
    char *rFile = checkRedirect(REDIRECT_FLAGS, in, args);
    //checking for & to run in bg
    for (int i = 0; i < strlen(in); i++)
    {
        if (in[i] == '&' && !in[i + 1])
        {
            // removing &
            //artificially ending the string
            in[i] = '\0';
            //setting background
            _BG = 1;
        }
    }

    //CD GOES HEREEEEEEEEEEEEEEEEEEEEEEE
    int _CNT = 0;
    // Extract the first token
    char *token = strtok(in, " ");
    //sentinals check
    // refactor
    if (!strcmp(token, "exit()"))
        exit(0);
    //stop parsing and return
    if (!strcmp(token, "!!"))
    {
        if (!argv[0])
            printf("History is Empty!\n");
        else
            print_argv(argv);
        //dont parse and return (redo last command)
        return rFile;
    }

    // loop through the string to extract all other tokens
    while (token != NULL)
    {
        // free strings from previous cycles to prevent leaks
        free(argv[_CNT]);
        // copy the token to argv
        // allocating to accomodate token
        argv[_CNT] = (char *)malloc(strlen(token) * sizeof(char));
        strcpy(argv[_CNT++], token);
        token = strtok(NULL, " ");
        argv[_CNT] = NULL;
    }

    return rFile;
}
// print command again after history exec
void print_argv(char *arg[MAX_LINE])
{
    for (size_t i = 0; i < MAX_LINE - 1; i++)
    {

        if (arg[i])
            printf("%s ", arg[i]);
        else
        {
            putchar('\n');
            break;
        }
    }
}

char *checkRedirect(int REDIRECT_FLAGS[], char *in, char *args[])
{
    for (int i = 0; i < strlen(in); i++)
    {
        // set appropriate redirect
        if (in[i] == '>')
        {
            REDIRECT_FLAGS[1] = 1;
            in[i] = ' ';
            char *rFile = (char *)malloc((strlen(in) - i + 1) * sizeof(char) * 5);
            int t = i;
            for (int j = 0; t < strlen(in); t++)
                if (in[t] != ' ')
                    rFile[j++] = in[t];
            for (int q = i; q < strlen(in); q++)
                in[q] = ' ';

            char c[50];
            // printf("\n\n%s\n\n", rFile);
            return rFile;
        }
        else if (in[i] == '<')
        {
            REDIRECT_FLAGS[0] = 1;
            in[i] = ' ';
            char *rFile = (char *)malloc((strlen(in) - i + 1) * sizeof(char) * 5);
            int t = i;
            for (int j = 0; t < strlen(in); t++)
                if (in[t] != ' ')
                    rFile[j++] = in[t];
            for (int q = i; q < strlen(in); q++)
                in[q] = ' ';

            // printf("\n\n%s\n\n", rFile);

            return rFile;
        }
        else if (in[i] == '|')
        {
            REDIRECT_FLAGS[2] = 1;
            in[i] = ' ';
            char *rFile = (char *)malloc((strlen(in) - i + 1) * sizeof(char) * 5);
            int t = i;
            for (int j = 0; t < strlen(in); t++)
                rFile[j++] = in[t];
            for (int q = i; q < strlen(in); q++)
                in[q] = ' ';

            toArgv(rFile, args);
            return rFile;
        }
    }
}

char **toArgv(char *str, char *args[MAX_LINE])
{
    int _CNT = 0;
    // Extract the first token
    char *token = strtok(str, " ");

    //stop parsing and return
    // loop through the string to extract all other tokens

    while (token != NULL)
    {
        // free strings from previous cycles to prevent leaks
        // free(args[_CNT]);

        // copy the token to args
        // allocating to accomodate token
        args[_CNT] = (char *)malloc(strlen(token) * sizeof(char));
        strcpy(args[_CNT++], token);
        token = strtok(NULL, " ");
        // printf("\n@%s@\n", args[_CNT]);
        args[_CNT] = NULL;
    }
    // for(int i = 0; i< _CNT; i++)
    // printf("\n\n----%s---\n\n", args[i]);

    // printf("\n\n\n\n");
    // print_argv(args);
    // printf("\n\n\n\n");
}

void prompt()
{
    char CWD[1024];
    getcwd(CWD, sizeof(CWD));
    char hName[1024];
    gethostname(hName, sizeof hName);

    printf(BHBLU "%s", getenv("USER"));
    printf("@" reset);
    printf(BMAG "[%s]:", hName);
    printf(reset);
    printf(BGRN "%s ", CWD);
    printf(reset);
    printf(RED " [%s]\n", __TIME__);
    printf(reset);
    printf(reset BHBLU ">" reset);
    printf(reset BHBLU ">" reset);
    printf(reset BHBLU "> " reset);
}
/*
        TODO
    process communication to send bg
    refactor excecution to fn
    check for redirect -- strsep
    add redierct 
    test 
    add pipe

    Add 7aniaka
*/
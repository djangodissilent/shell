#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

// #include "h.h"

//move to header file
#define MAX_LINE 80 /* The maximum length command */

int _BG = 0;

//char *routines to parse and print the argument vector
char *parse_input(char *in, char *argv[], int REDIRECT_FLAGS[]);
void print_argv(char *argv[MAX_LINE]);
// void sarrcpy(char *last[MAX_LINE], char *argv[]);
char *checkRedirect(int REDIRECT_FLAGS[], char *in);
int main(void)
{
    //redirecr Flags
    int REDIRECT_FLAGS[3];
    memset(REDIRECT_FLAGS, 0, sizeof REDIRECT_FLAGS);
    //input buffer
    char in[MAX_LINE];
    char *argv[MAX_LINE] = {0};
    char *last[MAX_LINE] = {0};

    int __RUN = 1;
    while (__RUN)
    {
        //NO REDIRECT default
        memset(REDIRECT_FLAGS, 0, sizeof REDIRECT_FLAGS);
        //  H for HAPDD
        printf("Hsh$ ");
        // fflush(stdout);
        char *rFile = parse_input(in, argv, REDIRECT_FLAGS);
        pid_t pid = fork();

        //fork failed!
        if (pid < 0)
        {
            fprintf(stderr, "Failed Fork");
            return 1;
        }
        else if (pid == 0)
        {
            if(REDIRECT_FLAGS[0]){
                int in;
                in = open(rFile, O_RDONLY);
                dup2(in, 0);
                close(in);
            }
            else if(REDIRECT_FLAGS[1]){
                int out;
                // printf("\n\n\n\n%s\n\n\n\n", rFile);
                out = open(rFile, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                // replace standard output with output file
                dup2(out, 1);
                close(out);
            }
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
        else
        {
            // wait for child
            if (!_BG)
                wait(NULL);
            else
            {
                printf("%d\n", pid);
            }
            continue;
        }
    }
    return 0;
}

char *parse_input(char *in, char *argv[MAX_LINE], int REDIRECT_FLAGS[3])
{
    //strtok modifies stirng
    char *tmp[MAX_LINE];

    scanf("%80[^\n]", in);
    getchar();
    char *rFile = checkRedirect(REDIRECT_FLAGS, in);
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
void print_argv(char *argv[MAX_LINE])
{
    for (size_t i = 0; i < MAX_LINE - 1; i++)
    {

        if (argv[i])
            printf("%s ", argv[i]);
        else
        {
            putchar('\n');
            break;
        }
    }
}

char *checkRedirect(int REDIRECT_FLAGS[], char *in)
{
    for (int i = 0; i < strlen(in); i++)
    {
        // set appropriate redirect
        if (in[i] == '>')
        {
            REDIRECT_FLAGS[1] = 1;
            in[i] = ' ';
            char *rFile = (char*)malloc((strlen(in) - i + 1) * sizeof(char)*5);
            int t = i;
            for(int j = 0; t < strlen(in); t++)
                if (in[t] != ' ')
                    rFile[j++] = in[t];
            for(int q = i; q < strlen(in); q++)
                in[q] = ' ';
            
            char c[50];
            printf("\n\n%s\n\n", rFile);
            return rFile;
        }
        else if(in[i] == '<'){
           REDIRECT_FLAGS[0] = 1;
            in[i] = ' ';
            char *rFile = (char*)malloc((strlen(in) - i + 1) * sizeof(char)*5);
            int t = i;
            for(int j = 0; t < strlen(in); t++)
                if (in[t] != ' ')
                    rFile[j++] = in[t];
            for(int q = i; q < strlen(in); q++)
                in[q] = ' ';
            
            // printf("\n\n%s\n\n", rFile);
            return rFile;
        }
        else if(in[i] == '|'){
           REDIRECT_FLAGS[2] = 1;
            in[i] = ' ';

            break;
        }
    }
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
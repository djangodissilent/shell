#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
// #include "h.h"

//move to header file
#define MAX_LINE 80 /* The maximum length command */

int _BG = 0;

// I/O routines to parse and print the argument vector
void parse_input(char *in, char *argv[]);
void print_argv(char *argv[MAX_LINE]);
// void sarrcpy(char *last[MAX_LINE], char *argv[]);

int main(void)
{
    //input buffer
    char in[MAX_LINE];
    char *argv[MAX_LINE] = {0};
    char *last[MAX_LINE] = {0};

    int __RUN = 1;
    while (__RUN)
    {
        //  H for HAPDD
        printf("Hsh$ ");
        // fflush(stdout);
        parse_input(in, argv);
        pid_t pid = fork();

        //fork failed!
        if (pid < 0)
        {
            fprintf(stderr, "Failed Fork");
            return 1;
        }
        else if (pid == 0)
        {
            //this commad traverses $PATH for argv[0]
            //and provides subsequent flags
            int ret = execvp(argv[0], argv); // Run argv[0] with argv[1..] flagss
            //set foreground
            //failed to find user command
            if (ret == -1)
                fprintf(stderr, "%s: command not found\n", argv[0]);
        }
        else
        {
            // wait for child
            if(!_BG) wait(NULL);
            else {
                printf("%d\n", pid);
        }
            continue;
        }
    }
    return 0;
}

void parse_input(char *in, char *argv[MAX_LINE])
{
    //strtok modifies stirng
    char *tmp[MAX_LINE];
    
    scanf("%80[^\n]", in);
    getchar();

    //checking for & to run in bg
    for (int i = 0; i < strlen(in); i++){
        if (in[i] == '&' && !in[i + 1])
        {
            // removing &
            //artificially ending the string
            in[i] = '\0';
            //setting background 
            _BG = 1;
        }
    }

    int _CNT = 0;
    // Extract the first token
    char *token = strtok(in, " ");
    //sentinals check
    // refactor
    if (!strcmp(token, "exit()"))
        exit(0);
    //stop parsing and return 
    if (!strcmp(token, "!!")){
        if(!argv[0]) printf("History is empty!\n");
        else print_argv(argv);
        //dont parse and return (redo last command)
        return;
    } 
    // loop through the string to extract all other tokens
    while (token != NULL)
    {
        // free strings from previous cycles to prevent leaks
        free(argv[_CNT]);
        // copy the token to argv
        // allocating to accomodate token
        argv[_CNT] = (char*)malloc(strlen(token)* sizeof (char));
        strcpy(argv[_CNT++], token);
        token = strtok(NULL, " ");
        argv[_CNT] = NULL;
        
    }

}
// print command again after history exec
void print_argv(char *argv[MAX_LINE])
{
    for (size_t i = 0; i < MAX_LINE - 1; i++){

        if (argv[i])
            printf("%s ", argv[i]);
        else {
            putchar('\n');
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
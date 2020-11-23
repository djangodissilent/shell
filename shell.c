#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "h.h"

//move to header file
#define MAX_LINE 80 /* The maximum length command */

int _BG = 0;

void parse_input(char *in, char *argv[]);

int main(void)
{
    char in[MAX_LINE];
    char *argv[MAX_LINE];
    int __RUN = 1;
    while (__RUN)
    {
        printf("Hshell>> ");
        fflush(stdout);
        parse_input(in, argv);
        if(!strcmp(argv[0] ,"exit()" )) exit(0);
        pid_t pid = fork();
        if (pid == 0)
        {
            int ret = execvp(argv[0], argv); // Run argv[0] with argv[1..] flagss
            //set flag to default 
            _BG = 0;
            if (ret == -1)
                fprintf(stderr, "%s: command not found\n", argv[0]);
        }
        else
        {
            // wait for child
            wait(NULL);
            continue;
        }
    }

    return 0;
}

void parse_input(char *in, char *argv[MAX_LINE])
{
    //strtok modifies stirng 
    char *tmp = in;
    scanf("%80[^\n]", in);
    getchar();
    int _CNT = 0;
    // Extract the first token
    char *token = strtok(in, " ");
    // loop through the string to extract all other tokens
    while (token != NULL)
    {
        argv[_CNT++] = token;
        token = strtok(NULL, " ");
        argv[_CNT] = NULL;
    }
    for (int i = 0; i < MAX_LINE - 1 && tmp[i]; i++)
        if (tmp[i] == '&' && !tmp[i + 1])
        {
            //artificially ending the string
            tmp[i] = '\0';
            //setting background flag to true
            _BG = 1;
        }
}
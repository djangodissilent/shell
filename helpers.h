#ifndef HELPERS_H

#define HELPERS_H

#define MAX_LINE 80 /* The maximum length command */



char *parse_input(char *in, char *argv[MAX_LINE], char *args[MAX_LINE], int REDIRECT_FLAGS[]);
void print_argv(char *argv[MAX_LINE]);
// void sarrcpy(char *last[MAX_LINE], char *argv[]);
char *checkRedirect(int REDIRECT_FLAGS[], char *in, char *args[]);
char **toArgv(char *in, char *args[MAX_LINE]);
void prompt();
int execute(char *argv[MAX_LINE], char *args[MAX_LINE], int REDIRECT_FLAGS[3], char *in);
#endif
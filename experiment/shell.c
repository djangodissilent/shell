#include "helpers.h"


int main(void)
{
    //redirecr Flags
    int REDIRECT_FLAGS[3];
    memset(REDIRECT_FLAGS, 0, 3 * sizeof REDIRECT_FLAGS[0]);
    //input buffer
    char in[MAX_LINE];
    char *argv[MAX_LINE] = {0};
    char *args[MAX_LINE] = {0};
    char *last[MAX_LINE] = {0};
    
    execute(argv, args, REDIRECT_FLAGS, in);
    return 0;
}
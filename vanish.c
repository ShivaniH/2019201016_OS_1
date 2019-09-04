#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "vanish.h"
#include "utilities.h"

int main()
{
    while(1)
    {
        char *command = (char*)malloc(sizeof(char)*INPUT_MAX);

        //ACCEPTING USER INPUT
        int index = 0;
        char input;
        scanf(" %c",&input);
        while(input != '\n')
        {
            command[index] = input;
            ++index;
            scanf(" %c",&input);
        }
        command[index] = '\0';

        char **commandWords = splitInput(command);

        __pid_t childPid = fork();
        if(childPid > 0)
        {
            // I am the parent

            int *childStatus;
            wait(childStatus);
            if(childStatus != 0)
            {
                printf("Could not execute command : %s \n",commandWords[0]);
            }
        }
        else if(childPid == 0)
        {
            //I am the child
            
            int exitStat = execvp(commandWords[0],commandWords);

            if(exitStat == -1)
            {
                exit(-1);
            }
            else {
                exit(0);
            }
        }

        free(command);
    }
    return 0;
}
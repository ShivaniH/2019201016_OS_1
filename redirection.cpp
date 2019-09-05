#include <stdio.h>
#include <iostream>
#include <limits>
#include <string.h>
#include <string>

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>
#include <termios.h>
#include <fcntl.h>
#include <vector>

#include "redirection.hpp"

using namespace std;

void outputRedirection(vector<string> toks, int i, string operat, const char *mode)
{
    FILE *fp = fopen(toks[++i].c_str(), mode);
    if(fp == NULL && operat == ">")
    {
        fp = fopen(toks[i].c_str(), "w");
    }

    int destFD = fileno(fp);

    pid_t childPid = fork();

    if(childPid > 0)
    {
        // I am the parent

        int childStatus;
        pid_t waitRet = waitpid(childPid, &childStatus, 0);

        if(waitRet != childPid)
        {
            //cout<<"Could not execute command: "<<commandWords[0]<<"\n";
            cout<<"Could not execute command\n";
        }
    }
    else if(childPid == 0)
    {
        //I am the child
        
        int shmid = shmget(SHM_KEY, MAXY_TOKENS*MAX_TOKEN_SIZE, SHM_PERM);
        if(shmid < 0)
        {
            cout<<"Could not get shared memory segment!\n";
            exit(EXIT_FAILURE);
        }

        char **commandWordZ = (char**)shmat(shmid, NULL, 0);

        if(commandWordZ == (char**)-1)
        {
            cout<<"Could not attach shared memory segment!\n";
            exit(EXIT_FAILURE);
        }

        if(dup2(destFD, STDOUT_FILENO) == -1)
        {
            cout<<"Could not redirect output!\n";
        }

        int exitStat = execvp(commandWordZ[0],commandWordZ);     
    }  
}
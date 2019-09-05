/**
 * Author: Shivani Hanji
 * Roll number: 2019201016
 * 
 */
#include <stdio.h>
#include <iostream>
#include <limits>
#include <string.h>
#include <string>

/*-------------------------------------
| POSIX header files              |
-------------------------------------*/
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>
#include <termios.h>
#include <fcntl.h>

//Refer: https://support.sas.com/documentation/onlinedoc/sasc/doc750/html/lr2/z2101586.htm

/*-------------------------------------
| Vani's header files              |
-------------------------------------*/
#include "vanish.hpp"
#include "utilities.hpp"
#include "background.hpp"


/******************************************* BACKGROUND DEALER *******************************************/


void backgroundDealer(vector<backgroundProc> &bg, int *fd)
{
    pid_t childPid = fork();

    if(childPid > 0)
    {
        // I am the parent. I don't wait for a background child
        backgroundProc bp(childPid);    //make an object for this background process

        bg.push_back(bp);

        //BAD!! pipe(fd);        
    }
    else if(childPid == 0)
    {
        //I am a child going to the background

        pid_t newSessionChildPid = setsid();

        if(newSessionChildPid == -1)
        {
            cout<<"There was an error creating a new session!\n";
            //exit(EXIT_FAILURE);
        }

        //cout<<"I'm about to become a background process, my pid is "<<getpid()<<"\n";

        //Shared memory segment to get command words
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

        //Shared memory segment to put saved STDOUT
        int shMemSegID = shmget((key_t)1098, sizeof(int), IPC_CREAT | SHM_PERM);
        if(shMemSegID < 0)
        {
            cout<<"Could not create shared memory segment!\n";
            exit(EXIT_FAILURE);
        }

        int *saveStdout = (int*)shmat(shMemSegID, NULL, 0);

        if(saveStdout == (int*)-1)
        {
            cout<<"Could not attach shared memory segment!\n";
            exit(EXIT_FAILURE);
        }
        
        *saveStdout = dup(STDOUT_FILENO);

        if(*saveStdout == -1)
        {
            cout<<"Could not save STDOUT of child moving to background!\n";
            exit(EXIT_FAILURE);
        }

        //bp.setSTDOUT(saveStdout);

        int devNull = open("/dev/null",O_WRONLY);
        if(devNull == -1)
        {
            cout<<"Could not open devnull!\n";
            exit(EXIT_FAILURE);
        }

        
        int dup2Result = dup2(devNull, STDOUT_FILENO);
        if(dup2Result == -1)
        {
            cout<<"Could not redirect background-transitioning child's output to devnull !\n";
            exit(EXIT_FAILURE);
        }

        /**
         * BAD!! with pipes
        if( dup2(STDOUT_FILENO, fd[1]) == -1 )
        {
            cout<<"Could not redirect background-transitioning child's output to pipe !\n";
            exit(EXIT_FAILURE);
        }
        */

        int exitStat = execvp(commandWordZ[0],commandWordZ);
        //cout<<"exitstat from child is "<<exitStat<<"\n";

        //close(devNull);

        /*
        if(exitStat == -1)
        {
            exit(-1);
        }
        else {
            exit(0);
        }
        */
    }
}
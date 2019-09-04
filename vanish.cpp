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

//Refer: https://support.sas.com/documentation/onlinedoc/sasc/doc750/html/lr2/z2101586.htm

/*-------------------------------------
| Vani's header files              |
-------------------------------------*/
#include "vanish.hpp"
#include "utilities.hpp"

using namespace std;

int main()
{
    int shmFlags = IPC_CREAT | SHM_PERM;

    while(true)
    {
        char *command = (char*)malloc(sizeof(char)*(INPUT_MAX));

        int sharedMemSegID = shmget(SHM_KEY, MAXY_TOKENS*MAX_TOKEN_SIZE, shmFlags);
        if(sharedMemSegID < 0)
        {
            cout<<"Could not create shared memory segment!\n";
            exit(EXIT_FAILURE);
        }

        char **commandWords;
        commandWords = (char**)shmat(sharedMemSegID, NULL, 0);   //NULL -> The system will automatically look for an address to attach the segment

        if(commandWords == (char**)-1)
        {
            cout<<"Could not attach shared memory segment!\n";
            exit(EXIT_FAILURE);
        }
        //ACCEPTING USER INPUT
        int index = 0;
        char input;
        //cin>>input;
        input = cin.get();

        //int testVar = 7;
        
        while(input != '\n' && input != '\t')
        {
            command[index] = input;
            ++index;
            //cin>>input;
            input = cin.get();
            //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            //cout<<"Just stuck here\n";
        }
        command[index] = '\0';

        int numTokens = 0;
        int &ref = numTokens;
        //char **commandWords = splitInput(ref, command);
        vector<string> toks;
        toks = splitInput(ref, command);
        for(int i = 0; i < numTokens; ++i)
        {
            //memcpy(commandWords[i],(char*)toks[i].c_str(), toks[i].length());
            commandWords[i] = (char*)toks[i].c_str();
        }
        
        //cout<<"number of tokens is "<<numTokens<<"\n";
        commandWords[numTokens] = NULL;
        ++numTokens;
        /*
        cout<<commandWords[0][0]<<"\n";
        cout<<"stuck here?\n";

        for(index = 0; index < numTokens-1; ++index)
        {
            cout<<"printing from vanish "<<commandWords[index]<<"\n";
        }
        */
        
        //cout<<"Reaching here at least! "<<commandWords[0]<<"\n";
        
        pid_t childPid = fork();
        if(childPid > 0)
        {
            // I am the parent

            //cout<<"Waiting for my child. . . \n";
            int *childStatus;
            //pid_t waitRet = wait(childStatus);
            pid_t waitRet = wait(NULL);
            /*
            cout<<"waitRet is "<<waitRet<<"\n";
            cout<<"childpid is "<<childPid<<"\n";
            */
            if(waitRet != childPid)
            {
                //cout<<"Could not execute command: "<<commandWords[0]<<endl;
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

            //cout<<"I can read testVar! : "<<testVar<<"\n";
            //cout<<"I am running "<<commandWordZ[0]<<"\n";
            //int exitStat = execvp(commandWords[0],commandWords);
            int exitStat = execvp(commandWordZ[0],commandWordZ);
            /*cout<<"exitstat from child is "<<exitStat<<"\n";

            if(exitStat == -1)
            {
                exit(-1);
            }
            else {
                exit(0);
            }
            */
        }

        free(command);
    }
    return 0;
}
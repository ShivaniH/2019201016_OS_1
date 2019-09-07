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
#include "redirection.hpp"
#include "piping.hpp"

using namespace std;

/****************************************** MAIN ******************************************************/

int main()
{
    struct termios term;

    tcgetattr (STDIN_FILENO, & term);
    term.c_lflag &= ~(ICANON);
    tcsetattr (STDIN_FILENO, TCSAFLUSH, & term);

    int shmFlags = IPC_CREAT | SHM_PERM;

    vector<backgroundProc> bg;

    //cout<<"pid of vanish = "<<getpid()<<" and group id of vanish = "<<getpgid(getpid());
    while(true)
    {
        char *command = (char*)malloc(sizeof(char)*(INPUT_MAX));

        int *fd;    //For piping in bg to fg

        int sharedMemSegID = shmget(SHM_KEY, MAXY_TOKENS*MAX_TOKEN_SIZE, shmFlags);
        if(sharedMemSegID < 0)
        {
            cout<<"Could not create shared memory segment!\n";
            exit(EXIT_FAILURE);
        }

        //cout<<"The shmid parent has is "<<sharedMemSegID<<"\n";

        char **commandWords;
        commandWords = (char**)shmat(sharedMemSegID, NULL, 0);   //NULL -> The system will automatically look for an address to attach the segment

        if(commandWords == (char**)-1)
        {
            cout<<"Could not attach shared memory segment!\n";
            exit(EXIT_FAILURE);
        }

        /*******************************ACCEPTING USER INPUT**************************************/
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
            //cout<<"Came back here\n";
        }
        command[index] = '\0';
        //cout<<"Really took the input\n";

        int numTokens = 0;
        int &ref = numTokens;
        //char **commandWords = splitInput(ref, command);
        vector<string> toks;
        toks = splitInput(ref, command);

        if(checkThisInTokens("&"))
        {
            /*********************************BACKGROUND PROCESS******************************************/

            for(int i = 0; i < numTokens; ++i)
            {
                if(toks[i] != "&") commandWords[i] = (char*)toks[i].c_str();
            }
            
            commandWords[numTokens-1] = NULL;

            fd = (int*)malloc(sizeof(int)*2);

            backgroundDealer(bg, fd);

            int shMSID = shmget((key_t)1098, sizeof(int), SHM_PERM);
            if(shMSID < 0)
            {
                cout<<"Could not fetch shared memory segment!\n";
                exit(EXIT_FAILURE);
            }

            int *saveSTDOUT = (int*)shmat(shMSID, NULL, 0);

            if(saveSTDOUT == (int*)-1)
            {
                cout<<"Could not attach shared memory segment!\n";
                exit(EXIT_FAILURE);
            }

            bg[bg.size()-1].setSTDOUT(*saveSTDOUT);
        
            //cout<<"I'm done here\n";
        }
        else if(checkThisInTokens("fg"))
        {
            /************************************BACK TO FOREGROUND*********************************************/
            signal(SIGTTOU, SIG_IGN);
            int childStatus;
            pid_t deadOrAlive;
            int i;
            for(i = 0; i < bg.size(); ++i)
            {
                deadOrAlive = bg[i].getBgId();
                cout<<"Checking process with pid = "<<deadOrAlive<<"\n";
                if( waitpid(deadOrAlive, &childStatus, WNOHANG) == 0 )    //this one's still running!
                {              
                    cout<<"I'm alive!\n";
                    break;
                }
            }
            if(i == bg.size())
            {
                cout<<"There is no process currently running in the background\n";
            }
            else {
                if(tcsetpgrp(STDIN_FILENO, getpgrp()) == -1)
                {
                    cout<<"Could not change background process' group to foreground group\n";
                }

                /**
                 * BAD! ATTEMPT WITH PIPE
                 * 
                if(dup2(STDOUT_FILENO, fd[0]) == -1)
                {
                    cout<<"Could not resume printing background process' output\n";
                }
                */

                /**
                 * ATTEMPT WITH fd in proc folder --- THIS seems to be the most likely to work!
                 * 
                 * 
                string filePath = "/proc/"+to_string(deadOrAlive)+"/fd/1";
                int outFD = open(filePath.c_str(), O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
                cout<<"outFD is "<<outFD<<"\n";
                if(outFD == -1)
                {
                    cout<<"Couldn't get access to background process' output\n";
                }

                char *buff;
                while(read(outFD, buff, 1) != 0)
                {
                    write(STDOUT_FILENO, buff, 1);
                }
                */
                            
                cout<<"STDOUT of background process is "<<bg[i].getSTDOUT()<<"\n";

                if(dup2(bg[i].getSTDOUT(), STDOUT_FILENO) == -1)
                {
                    cout<<"Could not resume printing background process' output\n";
                }
                
            }
        }
        else if(checkThisInTokens("cd"))
        {
            cout<<"path is "<<toks[2]<<"\n";
            if (chdir(toks[2].c_str()) != 0) 
            {
                cout<<"Please specify a path for cd\n";
            }
        }
        else if(checkThisInTokens("exit"))
        {
            return 0;
        }
        else if(checkThisInTokens(">"))
        {
            /************************************** OUTPUT REDIRECTION **********************************************/
            int i;
            for(i = 0; i < numTokens && toks[i] != ">"; ++i)
            {
                commandWords[i] = (char*)toks[i].c_str();
            }

            commandWords[i] = NULL;
            outputRedirection(toks, i, ">", "r+");
        }
        else if(checkThisInTokens(">>"))
        {
            /************************************** OUTPUT REDIRECTION APPEND **********************************************/
            int i;
            for(i = 0; i < numTokens && toks[i] != ">>"; ++i)
            {
                commandWords[i] = (char*)toks[i].c_str();
            }

            commandWords[i] = NULL;
            outputRedirection(toks, i, ">>", "a");
        }
        else if(checkThisInTokens("|"))
        {
            /********************************** PIPES ************************************************/
            int i, j, k;
            //int pipeFD[2];
            //pipe(pipeFD);
            //cout<<"pipefd[0] = "<<pipeFD[0]<<" "<<pipeFD[1]<<"\n";
            //int (&piperef)[2] = pipeFD;
            bool first = true;
            bool last = false;
            
            int numCommands = 1;
            for(i = 0; i < numTokens; ++i)
            {
                if(toks[i] == "|")
                {
                    ++numCommands;
                }
            }   

            int pipeFD[numCommands][2]; //read from current, write to next

            //test with: ls | grep c | grep v | wc
            
            int (*pipePtr)[2] = pipeFD;
            for(i = 0; i < numCommands; ++i)
            {
                pipe(pipeFD[i]);
                cout<<"pipe[0] =  "<<pipeFD[i][0]<<" pipe[1] = "<<pipeFD[i][1]<<"\n";
            }

            //int (&piperef)[][2] = pipeFD;
            

            for(i = 0, j = 0, k = 0; i < numTokens; ++i)
            {
                if(toks[i] != "|" )
                {
                    commandWords[k] = (char*)toks[i].c_str();
                    //cout<<"Stored "<<commandWords[k]<<" now\n";
                    ++k;
                }
                else
                {
                    commandWords[k] = NULL;
                    /*
                    cout<<"position k = "<<k<<" was set to NULL\n";
                    cout<<"There's a pipe now\n";
                    */
                    
                    k = 0;
                    piping(j, numCommands, pipePtr ,first, last);
                    ++j;
                    //cout<<"I'm gonna call piping for "<<commandWords[0]<<" now\n";
                    first = false;
                }
            }
            //for the final command
            last = true;
            commandWords[k] = NULL;
            k = 0;
            cout<<"I'm gonna call piping for "<<commandWords[0]<<" now\n";
            piping(j, numCommands, pipePtr ,first, last);
            cout<<"I'm out, bye bye\n";
        }
        else {
            /************************************NORMAL CASE****************************************/

            for(int i = 0; i < numTokens; ++i)
            {
                //memcpy(commandWords[i],(char*)toks[i].c_str(), toks[i].length());
                commandWords[i] = (char*)toks[i].c_str();
            }
            
            //cout<<"number of tokens is "<<numTokens<<"\n";
            commandWords[numTokens] = NULL;
            ++numTokens;

            pid_t childPid = fork();
            if(childPid > 0)
            {
                // I am the parent

                //cout<<"Waiting for my child. . . \n";
                int childStatus;
                //pid_t waitRet = wait(childStatus);
                pid_t waitRet = waitpid(childPid, &childStatus, 0);
                
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
                //cout<<"exitstat from child is "<<exitStat<<"\n";

                if(exitStat == -1)
                {
                    exit(-1);
                }
                else {
                    exit(0);
                }
                
            }
        }

        free(command);
        fflush(stdout);
    }
    return 0;
}
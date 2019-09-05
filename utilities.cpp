#include "utilities.hpp"
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

vector<string> tokens;
int count;

vector<string> splitInput(int &ref, char* str) 
{ 
    string command = str;
    //cout<<"The entire command is "<<str<<"\n";
    string word; 
    //char** tokens = ( char**)malloc(sizeof( char*) * MAXY_TOKENS);
    //string tokens[MAXY_TOKENS];
    /*
    for(i = 0; i < MAXY_TOKENS; ++i)
    {
        //tokens[i] = (char*)malloc(sizeof(char) * MAXY_TOKEN_SIZE);
    }
    */
    
    stringstream iss(command); 

    tokens.clear();
  
    count = 0;
    while (iss >> word) 
    {
        //char *currentWord = (char*)word.c_str();
        //tokens[i] = currentWord;
        tokens.push_back(word);
        //cout<<"current token being stored: "<<currentWord<<"\n";
        ++count;
    }
    //Now, i = number of tokens
    ref = count;

    /*
    for(int index = 0; index < i; ++index)
    {
        cout<<"printing from utilities: "<<tokens[index]<<"\n";
    }
    */

    return tokens; 
} 


bool checkThisInTokens(string special)
{
    for(int i = 0; i < count; ++i)
    {
        if(tokens[i] == special)
        {
            return true;
            break;
        }
    }
    return false;
}

// backgroundProc functions
backgroundProc::backgroundProc(pid_t backId)
{
    bgId = backId;
    exitStatus = 999;
}

backgroundProc::backgroundProc()
{ 
    exitStatus = 999;
}

pid_t backgroundProc::getBgId()
{
    return bgId;
}

int backgroundProc::getExitStatus()
{
    return exitStatus;
}

void backgroundProc::setExitStatus(int status)
{
    exitStatus = status;
}
  
void backgroundProc::setSTDOUT(int fd)
{
    stdoutFD = fd;
}

int backgroundProc::getSTDOUT()
{
    return stdoutFD;
}

#include "utilities.hpp"
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;
vector<string> splitInput(int &ref, char* str) 
{ 
    string command = str;
    //cout<<"The entire command is "<<str<<"\n";
    string word; 
    //char** tokens = ( char**)malloc(sizeof( char*) * MAXY_TOKENS);
    //string tokens[MAXY_TOKENS];
    vector<string> tokens;
    int i;
    /*
    for(i = 0; i < MAXY_TOKENS; ++i)
    {
        //tokens[i] = (char*)malloc(sizeof(char) * MAXY_TOKEN_SIZE);
    }
    */
    
    stringstream iss(command); 
  
    i = 0;
    while (iss >> word) 
    {
        //char *currentWord = (char*)word.c_str();
        //tokens[i] = currentWord;
        tokens.push_back(word);
        //cout<<"current token being stored: "<<currentWord<<"\n";
        ++i;
    }
    //Now, i = number of tokens
    ref = i;

    /*
    for(int index = 0; index < i; ++index)
    {
        cout<<"printing from utilities: "<<tokens[index]<<"\n";
    }
    */

    return tokens; 
} 
  

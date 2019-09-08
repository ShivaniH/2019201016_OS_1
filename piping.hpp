#include <string>
#include <vector>

#include "vanish.hpp"

#define MAXY_TOKENS 500
#define MAX_TOKEN_SIZE 100

using namespace std;

//int pipeFD[2];

void piping(int currentPipe, int numCommands, int (*pipePtr)[2], bool first, bool last, int &childStatus);
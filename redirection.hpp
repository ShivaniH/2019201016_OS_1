#include <string>
#include <vector>

#include "vanish.hpp"

#define MAXY_TOKENS 500
#define MAX_TOKEN_SIZE 100

using namespace std;

void outputRedirection(vector<string> toks, int i, string operat, const char *mode, int &childStatus);
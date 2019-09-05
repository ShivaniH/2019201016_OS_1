#include <iostream>
#include <limits>

using namespace std;

int main()
{
    int end = numeric_limits<int>::max();
    //int end = 999999;
    while(end >= 0)
    {
        cout<<"meow  ";
        --end;
    }
}
#include "State.h"
#include <set>

using namespace std;

#ifndef __NFA__
#define __NFA__

class NFA
{   
    public:
    State* acceptingState;
    State* startState;
    set<char> possibleInputs;
};

#endif


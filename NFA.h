#include "State.h"

using namespace std;

#ifndef __NFA__
#define __NFA__

class NFA
{   
    public:
    State* acceptingState;
    State* startState;
};

#endif


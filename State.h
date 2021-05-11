#include <vector>

#ifndef STATE_H
#define STATE_H

using namespace std;

class State;

class Edge
{   
    public:
        State* to;
        char guard;

};

class State
{   
    public:
        int id;
        vector<Edge*> nextStates;
        vector<State*> epsilonClosure;
};

#endif
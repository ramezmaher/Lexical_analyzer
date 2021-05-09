#include <vector>
#include <string>
#include "DFA_state.h"


#ifndef __DFA__
#define __DFA__

class DFA{
    vector<DFA_state*> states;
    bool is_minimized;
    DFA_state* dead_state;
    public:
        DFA(vector<char> inputs,vector<DFA_state*> all_states);
        void minimize();
        void print_states(); 
};

#endif
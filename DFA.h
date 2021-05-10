#include <vector>
#include <string>
#include <unordered_set>
#include "DFA_state.h"


#ifndef __DFA__
#define __DFA__

class DFA{
    vector<DFA_state*> states;
    DFA_state* start_state;
    DFA_state* dead_state;
    int start_state_id;
    int dead_state_id;
    public:
        DFA();
        DFA(vector<char> inputs,vector<DFA_state*> all_states);
        void print_states();
        DFA_state* get_start_state();
        DFA_state* get_dead_state();
        void delete_DFA();

    private:
        bool check_sizes(vector<unordered_set<DFA_state*>> v);
        void minimize(); 
};

#endif
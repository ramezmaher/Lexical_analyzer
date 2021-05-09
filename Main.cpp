#include <vector>
#include <string>
#include <iostream>
#include "DFA_state.h"
#include "DFA.h"

int main(){
    vector<DFA_state*> all_states;
    DFA_state* state1 = new DFA_state(false,1);
    DFA_state* state2 = new DFA_state(false,2);
    DFA_state* state3 = new DFA_state(true,3);

    vector<char> inputs;
    inputs.push_back('0');
    inputs.push_back('1');

    state1->add_direction('0',state2);
    state1->add_direction('1',state3);
    all_states.push_back(state1);

    state2->add_direction('0',state3);
    state2->add_direction('1',state3);
    all_states.push_back(state2);

    state3->add_direction('0',state1);
    state3->add_direction('1',state3);
    all_states.push_back(state3);


    DFA table(inputs,all_states);
    table.minimize();
    table.print_states();

}
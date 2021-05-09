#include <vector>
#include <string>
#include "DFA_state.h"
#include "DFA.h"

int main(){
    vector<DFA_state*> all_states;
    for(int i = 1; i <= 10; i++){
        DFA_state* state = new DFA_state(false,i);
        all_states.push_back(state);
    }

    vector<char> inputs;
    inputs.push_back('0');
    inputs.push_back('1');

    for(int i = 1; i < 9; i++){
        all_states.at(i)->add_direction('0',all_states.at(i-1));
        all_states.at(i)->add_direction('1',all_states.at(i+1));
    }

    DFA table(inputs,all_states);
    table.print_states(); 
}
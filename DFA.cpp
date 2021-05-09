#include <iostream>
#include <vector>
#include <string>
#include "DFA_state.h"
#include "DFA.h"

DFA::DFA(vector<string> inputs,vector<DFA_state*> all_states){
    //Initilize dead state
    DFA_state* dead_state = new DFA_state(false,0);

    //For all possible inputs, if a state does not have a valid transition, make a transition to dead_state. 
    //Initialize dead_state's transitions under all inputs to itself
    for(auto input: inputs){
         dead_state->add_direction(input,dead_state);
         for(auto state: all_states){
             if(state->edges.find(input) == state->edges.end()){
                 state->edges.insert(make_pair(input,dead_state));
             }
         }
    }
    //add dead state to the table;
    states.push_back(dead_state);

    //add all the states to the DFA table 
    for(auto state: all_states){
        states.push_back(state);
    }

    //checks if table is minimized, currently it has only the dead_state, hence it does not require minimization
    is_minimized = false;
}
 
void DFA::print_states(){
    for(auto state: states){
        printf("%d --->  ",state->get_id());
        cout << state.
    }
}

int main(){

}




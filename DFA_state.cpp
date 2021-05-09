#include <iostream>
#include <string>
#include <unordered_map>
#include "DFA_state.h"

using namespace std;


DFA_state::DFA_state(bool accepting,int num){
    id = num;
    is_accepting = accepting;
}

bool DFA_state::isAccepting(){
    return is_accepting;
}

void DFA_state::add_direction(string input,DFA_state* target){
    edges[input] = target;
}

int DFA_state::get_id(){
    return id;
}

bool DFA_state::compare(DFA_state* state){
    return (edges == state->edges && (is_accepting ^ state->isAccepting()) == 0);
}

void DFA_state::print_dir(){
    for(auto transition: edges){
        cout << "\t" << transition.first << " --> " << transition.second->get_id() << endl;
    }
}
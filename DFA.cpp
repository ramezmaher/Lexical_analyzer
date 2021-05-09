#include <iostream>
#include <vector>
#include <unordered_set>
#include "DFA_state.h"
#include "DFA.h"


DFA::DFA(vector<char> inputs,vector<DFA_state*> all_states){
    //Initilize dead state
    DFA_state* dead_state = new DFA_state(false,0);

    //For all possible inputs, if a state does not have a valid transition, make a transition to dead_state. 
    //Initialize dead_state's transitions under all inputs to itself
    bool is_dead_used = false;
    for(auto input: inputs){
         dead_state->add_direction(input,dead_state);
         for(auto state: all_states){
             if(state->edges.find(input) == state->edges.end()){
                 is_dead_used = true;
                 state->edges.insert(make_pair(input,dead_state));
             }
         }
    }
    //add dead state to the table;
    if(is_dead_used)
        states.push_back(dead_state);

    //add all the states to the DFA table 
    for(auto state: all_states){
        states.push_back(state);
    }

    //checks if table is minimized, currently it has only the dead_state, hence it does not require minimization
    is_minimized = false;
}
 
void DFA::print_states(){
    if(states.empty()){
        cout << "nothing" << endl;
    }
    for(auto state: states){
        cout << state->get_id() << ":-" << endl;
        state->print_dir();
        cout << "**********" <<  endl;
    }
}

void DFA::minimize(){
    // check if no minimization required
    if(is_minimized)
        return;

    //vector to hold each of the partions levels
    vector<unordered_set<DFA_state*>> level;

    unordered_set<DFA_state*> accepting_states;
    unordered_set<DFA_state*> nonaccepting_states;

    //Partiotion states to accepting states and nonaccepting states 
    for(auto state: states){
        if(state->isAccepting()){
            accepting_states.insert(state);
        }
        else{
            nonaccepting_states.insert(state);
        }
    }

    //check if no minimataion required. Either both of them is empty or contains only one state or one of them is empty and the other contains one state
    if((accepting_states.size() == 1 && nonaccepting_states.size() == 1) || (accepting_states.empty() && nonaccepting_states.empty()) || ((accepting_states.empty() || nonaccepting_states.empty()) && (accepting_states.size() == 1 || nonaccepting_states.size() == 1))){
            is_minimized = true;
            return;
    }

    level.push_back(nonaccepting_states);
    level.push_back(accepting_states);

    //this loop will serve to partition all the states into the minimum number of sets 
    while(true){
        
        //if all sets contains only one state, then there is no need to minimize.
        if(check_sizes(level))
            break;
        //vector to hold next level's sets    
        vector<unordered_set<DFA_state*>> next_level;

        for(auto set: level){
            //to keep track of the transitions of set elements to other sets
            unordered_map<DFA_state*,unordered_map<char,int>> map;
            for(auto state: set){
                unordered_map<char,int> sub_map;
                for(auto edge: state->edges){
                    char c = edge.first;
                    DFA_state* transition = edge.second;
                    int set_num;
                    for (int i = 0; i < level.size(); i++ ){
                        if(level.at(i).find(transition) != level.at(i).end()){
                            set_num = i;
                            break;
                        }
                    }
                    sub_map[c] = set_num; 
                }
                map[state] = sub_map;
            }
            //check if depending on the transitions of the states they should be subdivided
            vector<unordered_set<DFA_state*>> transient;
            for(auto entry: map){
                //Add set if the vector is empty
                if(transient.empty()){
                    unordered_set<DFA_state*> s;
                    s.insert(entry.first);
                    transient.push_back(s);
                }
                else{
                    //checks all the current sets if this entry could be a part of it
                    bool included = false;
                    for(int k = 0; k < transient.size(); k++){
                        bool is_in_set = true;
                        //for this entry to be a part of this set, it must have the same accpetance value, and under same input
                        //goes to same sets
                        for(auto dfa_state_t: transient.at(k)){
                            if(entry.second != map[dfa_state_t] || (entry.first->isAccepting() ^ dfa_state_t->isAccepting()) == 1){
                                is_in_set = false;
                                break;
                            }
                        }
                        //if this boolean is true then this entry could be part of this set
                        if(is_in_set){
                            included = true;
                            transient.at(k).insert(entry.first);
                            break;
                        }
                    }
                    //this indicates that the entry takes no part of any set and hence needs to be added to a new set
                    if(!included){
                        unordered_set<DFA_state*> s;
                        s.insert(entry.first);
                        transient.push_back(s);
                    }
                }
            }
            //add all transient sets to the next level
            for(auto element: transient){
                next_level.push_back(element);
            }

        } 

        //if the next level has the same sets as this one, then no further minimization is possible, and hence termminate
        if(next_level == level)
            break;
        else
            level = next_level;
        
    }

    //clear old states
    states.clear();

    //convert all sets to states, and put them in the table
    for(int i = 0; i < level.size(); i++){
        DFA_state* state;
        for(auto elem: level.at(i)){
            state = new DFA_state(elem->isAccepting(),i);
            break;
        }
        states.push_back(state);
    }

    //adjust states transitions
    for(int i = 0; i < level.size(); i++){
        for(auto x: level.at(i)){
            for(auto edge: x->edges){
                char c = edge.first;
                int target;
                for(int j = 0; j < level.size(); j++){
                    if(level.at(j).find(edge.second) != level.at(j).end()){
                        target = j;
                        break;
                    }
                }
                states.at(i)->add_direction(c,states.at(target));
            }
            break;
        }
    }

    is_minimized = true;

}

bool DFA::check_sizes(vector<unordered_set<DFA_state*>> v){
    for(auto set: v){
        if(set.size() > 1)
            return false;
    }
        return true;   
}
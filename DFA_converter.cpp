#include <iostream>
#include <unordered_set>
#include <queue>
#include <set>
#include <string>
#include <map>
#include <unordered_map>
#include "DFA.h"
#include "DFA_converter.h"
#include "NFA.h"
#include "DFA_state.h"


DFA* DFA_converter::convert_NFA(NFA* nfa,set<char> input){

    //contains all the possible sets of the NFA
    unordered_set<unordered_set<State*>*> all_sets;

    map<unordered_set<State*>*,map<char,unordered_set<State*>*>*> nfa_table;
    //contains only the unvisited sets of the NFA
    queue<unordered_set<State*>*> unvisited_sets;

    //Begin chunk: for each of the unvisited states of the set, visit them and add to
    //the unvisited queue all their epsilon transitions
    queue<State*> not_visited;
    unordered_set<State*> visited;

    not_visited.push(nfa->startState);
    while(!not_visited.empty()){
        visit_all_epsilon(&not_visited,&visited,not_visited.front());
    }

    //now that we visited all the epsilon transitions, add this set as the first unvisited set for all the inputs
    unordered_set<State*> first_set = visited;
    unvisited_sets.push(&first_set);
    all_sets.insert(&first_set);

    //now we'll check each unvisited set, test it against all possible inputs, if the transition of this set is not included in all sets,
    //we add it as unvisited. Otherwise, it was visited or in the queue waiting to be visited.
    while(!unvisited_sets.empty()){ 
        map<char,unordered_set<State*>*>* tuple = new map<char,unordered_set<State*>*>();
        for(auto guard: input){
            for(auto elem: *unvisited_sets.front()){
                for(auto edge: elem->nextStates){
                    if(edge->guard == guard)
                        not_visited.push(edge->to);
                }
            }
            //check if any of the element has epsilon transitions
            unordered_set<State*>* set = new unordered_set<State*>();
            while(!not_visited.empty()){
                visit_all_epsilon(&not_visited,set,not_visited.front());
            }
            bool found;
            unordered_set<State*>* found_set;
            for(auto x: all_sets){
                found = false;
                if(x->size() == set->size()){
                    found = true;
                    for(auto set_elem: *x){
                        if(set->find(set_elem) == set->end()){
                            found = false;
                            break;
                        }
                    }
                }
                if(found){
                    found_set = x;
                    break;
                }
            }
            if(!found){
                all_sets.insert(set);
                unvisited_sets.push(set);
                tuple->insert(make_pair(guard,set));
            }
            else{
                tuple->insert(make_pair(guard,found_set));
                delete set;
            }
        }
        nfa_table.insert(make_pair(unvisited_sets.front(),tuple));
        unvisited_sets.pop();
    }

    State* accepting_state = nfa->acceptingState;
    unordered_set<State*>* empty_set = NULL;
    bool has_empty = false;
    for(auto x : nfa_table){
        if(x.first->size() == 0){
            has_empty = true;
            empty_set = x.first;
        }
    }
    int i = 0;
    vector<DFA_state*> v;
    map<unordered_set<State*>*,int> map;
    for(auto x : nfa_table){
        if(x.first != empty_set){
            bool is_accpepting = false;
            for(auto y: *x.first){
                if(y == accepting_state){
                    is_accpepting = true;
                }
            }
            map[x.first] = i;
            DFA_state* state = new DFA_state(is_accpepting,i);
            v.push_back(state);
            i++;
        }
    }
    for(auto x: nfa_table){
        if(x.first != empty_set){
            for(auto y: *x.second){
                if(y.second != empty_set){
                    v.at(map[x.first])->add_direction(y.first,v.at(map[y.second]));
                }
            }
        }
    }
    
    DFA* dfa = new DFA(input,v);
    return dfa;

}

void DFA_converter::visit_all_epsilon(queue<State*>* not_visited,unordered_set<State*>* visited,State* state){
    //check if already visited this node
    if(visited->find(state) != visited->end())
        return;
    visited->insert(state);
    for(auto x: state->epsilonClosure)
        if(visited->find(x) == visited->end())
            not_visited->push(x);
    not_visited->pop();  
}

#include <unordered_set>
#include <queue>
#include <vector>
#include "DFA_state.h"
#include "DFA.h"
#include "NFA.h"

#ifndef __DFA_CONVERTER__
#define __DFA_CONVERTER__

using namespace std;

class DFA_converter{
    public:
        DFA* convert_NFA(NFA* nfa);
    private:
        void visit_all_epsilon(queue<State*>* not_visited,unordered_set<State*>* visited,State* state);
};


#endif
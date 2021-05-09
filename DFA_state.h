#include <unordered_map>

using namespace std;

#ifndef __DFA_STATE__
#define __DFA_STATE__

class DFA_state{

    private:
        bool is_accepting;
        int id;

    public:
        unordered_map<char,DFA_state*> edges;
        DFA_state(bool accepting,int num);
        bool isAccepting();
        void add_direction(char input,DFA_state* target);
        int get_id();
        bool compare(DFA_state* B);
        void print_dir();

};

#endif
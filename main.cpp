#include <set>
#include "NFA.h"
#include "DFA_converter.h"
#include "NFA_maker.h"
#include "DFA.h"

int main()
{
    string regularExpression = "a(a|b)*|c";
    /*string regularExpression2 = "(a|b(c|d|e|f)*bc)*de|g";
    string regularExpression3 = "ab*c";
    string regularExpression4 = "ab*|cd+|gk";
    string regularExpression5 = "ab";
    string regularExpression6 = "ab*|bc+";*/

    NFA_maker* make = new NFA_maker();

    NFA* nfa = make->getNFA(regularExpression);
    
    set<char> set = {'a','b','c'};

    DFA_converter* dfac = new DFA_converter();
    DFA* dfa = dfac->convert_NFA(nfa,set);
    dfa->print_states();

    return 0;
}
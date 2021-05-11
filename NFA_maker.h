#include <string>
#include <map>
#include <vector>
#include "NFA.h"
#include "NFA.h"
#include "State.h"


#ifndef __NFA_MAKER__
#define __NFA_MAKER__

class NFA_maker{
    public:
        NFA* getNFAindexed(string regularExpression, int start, int end);
        NFA* getNFA(string regularExpression);
        int getNextID();
        string removeShortcuts(string a);
        map<string, string> getRegularDefintions(vector<string> strs);
        string expandRegularExpression(string regularExpression, map<string,string> regularDefinitions);    
        void showNFA(NFA* nfa);
};

#endif
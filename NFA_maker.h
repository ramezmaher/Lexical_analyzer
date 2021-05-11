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
        void buildNFAInputs(NFA* nfa);
        int getNextID();
        void showNFA(NFA* nfa);
};

#endif

#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <stack>
#include "NFA.h"
#include "State.h"
#include "NFA_maker.h"

using namespace std;

map<string,string> regularDefinitions;

int NFA_maker::getNextID()
{
    static int id = 0;
    return id++;
}

// Removes shortcuts in regular definition (0-9 , a-z , ...)
string NFA_maker::removeShortcuts(string a)
{
    static string lowerCaseLetters = "a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z";
    static string upperCaseLetters = "A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z";
    static string nums = "0|1|2|3|4|5|6|7|8|9";

    int pos = a.find("a-z");
    if (pos != -1)
        a.replace(pos, 3, lowerCaseLetters);

    pos = a.find("A-Z");
    if (pos != -1)
        a.replace(pos, 3, upperCaseLetters);

    pos = a.find("0-9");
    if (pos != -1)
        a.replace(pos, 3, nums);

    return a;
}

string NFA_maker::expand(string s)
{
    map<string, string>::iterator itr;
    for (itr = regularDefinitions.begin(); itr != regularDefinitions.end(); itr++)
    {
        string regularDefinition = itr->first;
        int pos = s.find(regularDefinition);
        while (pos != -1 && s[pos + regularDefinition.size()] != 's')
        {
            string expansion = '(' + itr->second + ')';
            s.replace(pos,regularDefinition.size(), expansion);
            pos = s.find(regularDefinition);
        }
    }
    return s;
}

NFA* NFA_maker::getNFAindexed(string regularExpression, int start, int end)
{
    NFA* result = new NFA;;

    if (start == end)
    {
        State* s0 = new State;
        s0->id = getNextID();
        NFA* nfaInitial = new NFA;
        nfaInitial->startState = s0;
        nfaInitial->acceptingState = s0;
        return nfaInitial;
    }

    int isBuilt = 0;
    for (int i = start; i < end; i++)
    {

        if (regularExpression[i] == '(')
        {
            int brackets = 1;
            int j;
            for (j = i + 1; j < end; j++)
            {
                if (regularExpression[j] == '(')
                    brackets++;
                else if (regularExpression[j] == ')')
                    brackets--;

                if (brackets == 0)
                {
                    i = j + 1;
                    break;
                }

            }

        }

        // Recursively build NFA(start,i-1) and NFA(i+1, end) then OR them together
        else if (regularExpression[i] == '|')
        {
            isBuilt = 1;
            NFA* nfa1 = getNFAindexed(regularExpression, start, i);
            NFA* nfa2 = getNFAindexed(regularExpression, i + 1, end);

            State* newStartState = new State;
            newStartState->id = getNextID();
            State* newEndState = new State;
            newEndState->id = getNextID();

            Edge* e1 = new Edge;
            e1->to = nfa1->startState;
            e1->guard = 'ε';
            newStartState->nextStates.push_back(e1);
            newStartState->epsilonClosure.push_back(e1->to);

            Edge* e2 = new Edge;
            e2->to = nfa2->startState;
            e2->guard = 'ε';
            newStartState->nextStates.push_back(e2);
            newStartState->epsilonClosure.push_back(e2->to);

            Edge* e3 = new Edge;
            e3->to = newEndState;
            e3->guard = 'ε';
            nfa1->acceptingState->nextStates.push_back(e3);
            nfa1->acceptingState->epsilonClosure.push_back(e3->to);

            Edge* e4 = new Edge;
            e4->to = newEndState;
            e4->guard = 'ε';
            nfa2->acceptingState->nextStates.push_back(e4);
            nfa2->acceptingState->epsilonClosure.push_back(e4->to);

            result->startState = newStartState;
            result->acceptingState = newEndState;

            return result;
        }
    }

    if (isBuilt == 0)
    {
        State* s0 = new State;
        s0->id = getNextID();

        NFA* nfaInitial = new NFA;
        nfaInitial->startState = s0;
        nfaInitial->acceptingState = s0;

        NFA* prevNFA = nfaInitial;
        for (int i = start; i < end; i++)
        {
            if (regularExpression[i] == ' ')
                continue;
            int skipped;
            int isLambda = 0;

            if (regularExpression[i] == '\\' && regularExpression[i-1] != '\\') {
                i++;
                skipped = 1;
                if (regularExpression[i] == 'L')
                    isLambda = 1;

            } else
                skipped = 0;

            if (regularExpression[i] == '(' && skipped == 0)
            {
                int brackets = 1;
                int j;
                for (j = i + 1; j < end; j++)
                {
                    if (regularExpression[j] == '(')
                        brackets++;
                    else if (regularExpression[j] == ')')
                        brackets--;

                    if (brackets == 0)
                    {
                        break;
                    }
                }
                NFA* nfaBrackets = getNFAindexed(regularExpression, i+1, j);

                State* sNewAccepting = new State;
                sNewAccepting->id = getNextID();

                Edge* e1 = new Edge;
                e1->to = nfaBrackets->startState;
                e1->guard = 'ε';
                prevNFA->acceptingState->nextStates.push_back(e1);
                prevNFA->acceptingState->epsilonClosure.push_back(e1->to);

                Edge* e2 = new Edge;
                e2->to = sNewAccepting;
                e2->guard = 'ε';
                nfaBrackets->acceptingState->nextStates.push_back(e2);
                nfaBrackets->acceptingState->epsilonClosure.push_back(e2->to);

                i = j;

                if (regularExpression[j+1] == '*' || regularExpression[j+1] == '+')
                {
                    i++;
                    Edge* e3 = new Edge;
                    e3->to = nfaBrackets->startState;
                    e3->guard = 'ε';
                    nfaBrackets->acceptingState->nextStates.push_back(e3);
                    nfaBrackets->acceptingState->epsilonClosure.push_back(e3->to);

                    if (regularExpression[j+1] == '*')
                    {
                        Edge* e4 = new Edge;
                        e4->to = sNewAccepting;
                        e4->guard = 'ε';
                        prevNFA->acceptingState->nextStates.push_back(e4);
                        prevNFA->acceptingState->epsilonClosure.push_back(e4->to);
                    }
                }
                prevNFA->acceptingState = sNewAccepting;
            }
            else
            {
                State* sNew = new State;
                sNew->id = getNextID();

                Edge* eNew = new Edge;
                eNew->to = sNew;
                if (isLambda == 1) {
                    eNew->guard = 'ε';
                    prevNFA->acceptingState->epsilonClosure.push_back(eNew->to);
                }
                else {
                    eNew->guard = regularExpression[i];
                    char* c = new char;
                    *c = regularExpression[i];
                    prevNFA->possibleInputs.insert(*c);
                }

                prevNFA->acceptingState->nextStates.push_back(eNew);

                if (regularExpression[i+1] == '*' || regularExpression[i+1] == '+')
                {
                    State* newAcceptingState = new State;
                    newAcceptingState->id = getNextID();
                    Edge* e1 = new Edge;
                    e1->to = newAcceptingState;
                    e1->guard = 'ε';
                    sNew->nextStates.push_back(e1);
                    sNew->epsilonClosure.push_back(e1->to);


                    Edge* e2 = new Edge;
                    e2->to = prevNFA->acceptingState;
                    e2->guard = 'ε';
                    newAcceptingState->nextStates.push_back(e2);
                    newAcceptingState->epsilonClosure.push_back(e2->to);

                    if (regularExpression[i+1] == '*')
                    {
                        Edge* e3 = new Edge;
                        e3->to = newAcceptingState;
                        e3->guard = 'ε';
                        prevNFA->acceptingState->nextStates.push_back(e3);
                        prevNFA->acceptingState->epsilonClosure.push_back(e3->to);
                    }

                    prevNFA->acceptingState = newAcceptingState;
                    i++;
                }
                else
                {
                    prevNFA->acceptingState = sNew;
                }
            }

        }
        result = prevNFA;
    }
    return result;
}
    
void NFA_maker::buildNFAInputs(NFA* nfa) {
    stack<State*> statesToVisit;
    set<int> visited;

    visited.insert(nfa->startState->id);
    statesToVisit.push(nfa->startState);
    while (!statesToVisit.empty())
    {
        State* current = statesToVisit.top();
        statesToVisit.pop();
        vector<Edge*> neighbors = current->nextStates;
        for (int i = 0; i < neighbors.size(); i++)
        {
            Edge* neighbor = neighbors[i];
            if (neighbor->guard != 'ε') {
                char* c = new char;
                *c = neighbor->guard;
                nfa->possibleInputs.insert(*c);
            }

            if (visited.count(neighbor->to->id) == 0)
            {
                visited.insert(neighbor->to->id);
                statesToVisit.push(neighbor->to);
            }
        }
    }
}

NFA* NFA_maker::getNFA(string regularExpression)
{
    NFA* result = getNFAindexed(regularExpression, 0, regularExpression.length());
    buildNFAInputs(result);
    return result;
}

void NFA_maker::showNFA(NFA* nfa)
{
    stack<State*> statesToVisit;
    set<int> visited;

    visited.insert(nfa->startState->id);
    statesToVisit.push(nfa->startState);
    while (!statesToVisit.empty())
    {
        State* current = statesToVisit.top();
        statesToVisit.pop();
        cout << "FROM STATE " << current->id << ", WE CAN VISIT: " << endl;
        vector<Edge*> neighbors = current->nextStates;
        for (int i = 0; i < neighbors.size(); i++)
        {
            Edge* neighbor = neighbors[i];
            cout << "state: " << neighbor->to->id << ", for input " << neighbor->guard << endl;
            if (visited.count(neighbor->to->id) == 0)
            {
                visited.insert(neighbor->to->id);
                statesToVisit.push(neighbor->to);
            }
        }
        cout << "Epsilon Closure: ";
        vector<State*> epsClosure = current->epsilonClosure;
        for (int i = 0; i < epsClosure.size(); i++) {
            State* s = epsClosure[i];
            cout << s->id << "\t";
        }

        cout << "\n\n";
    }

    cout << "Start State is: " << nfa->startState->id << ", Accepting State is: " << nfa->acceptingState->id << endl;
}



#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <vector>
#include <bits/stdc++.h>
#include <string>


using namespace std;

struct NFA;
struct Edge;
struct State;

struct State
{
    int id;
    vector <struct Edge*> nextStates;

};

struct Edge
{
    struct State* to;
    char guard;

};

struct NFA
{
    struct State* acceptingState;
    struct State* startState;
};

int getNextID()
{
    static int id = 0;
    return id++;
}

// Removes shortcuts in regular definition (0-9 , a-z , ...)
string removeShortcuts(string a)
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

map<string, string> getRegularDefintions(vector<string> strs)
{
    map<string, string> regularDefinitions;

    for (int i = 0; i < strs.size(); i++)
    {
        string current = strs[i];

        int pos = current.find("=");
        if (pos != -1)
        {
            current = removeShortcuts(current);
            string part1 = current.substr(0, pos - 1);
            string part2 = current.substr(pos + 2);

            regularDefinitions.insert(pair<string, string>(part1, part2));
        }
    }
    return regularDefinitions;
}

string expandRegularExpression(string regularExpression, map<string,string> regularDefinitions)
{
    map<string, string>::iterator itr;
    for (itr = regularDefinitions.begin(); itr != regularDefinitions.end(); itr++)
    {
        string regularDefinition = itr->first;
        int pos = regularExpression.find(regularDefinition);
        if (pos != -1)
        {
            regularExpression.replace(pos,regularDefinition.size(), itr->second);
        }
    }
    return regularExpression;
}


struct NFA* getNFAindexed(string regularExpression, int start, int end)
{
    struct NFA* result = new NFA;;

    if (start == end)
    {
        struct State* s0 = new State;
        s0->id = getNextID();
        struct NFA* nfaInitial = new NFA;
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
            struct NFA* nfa1 = getNFAindexed(regularExpression, start, i);
            struct NFA* nfa2 = getNFAindexed(regularExpression, i + 1, end);

            struct State* newStartState = new State;
            newStartState->id = getNextID();
            struct State* newEndState = new State;
            newEndState->id = getNextID();

            struct Edge* e1 = new Edge;
            e1->to = nfa1->startState;
            e1->guard = 'ε';
            newStartState->nextStates.push_back(e1);

            struct Edge* e2 = new Edge;
            e2->to = nfa2->startState;
            e2->guard = 'ε';
            newStartState->nextStates.push_back(e2);

            struct Edge* e3 = new Edge;
            e3->to = newEndState;
            e3->guard = 'ε';
            nfa1->acceptingState->nextStates.push_back(e3);

            struct Edge* e4 = new Edge;
            e4->to = newEndState;
            e4->guard = 'ε';
            nfa2->acceptingState->nextStates.push_back(e4);

            result->startState = newStartState;
            result->acceptingState = newEndState;

            return result;
        }
    }

    if (isBuilt == 0)
    {
        struct State* s0 = new State;
        s0->id = getNextID();

        struct NFA* nfaInitial = new NFA;
        nfaInitial->startState = s0;
        nfaInitial->acceptingState = s0;

        struct NFA* prevNFA = nfaInitial;
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
                        break;
                    }
                }
                struct NFA* nfaBrackets = getNFAindexed(regularExpression, i+1, j);

                struct State* sNewAccepting = new State;
                sNewAccepting->id = getNextID();

                struct Edge* e1 = new Edge;
                e1->to = nfaBrackets->startState;
                e1->guard = 'ε';
                prevNFA->acceptingState->nextStates.push_back(e1);

                struct Edge* e2 = new Edge;
                e2->to = sNewAccepting;
                e2->guard = 'ε';
                nfaBrackets->acceptingState->nextStates.push_back(e2);

                i = j;

                if (regularExpression[j+1] == '*' || regularExpression[j+1] == '+')
                {
                    i++;
                    struct Edge* e3 = new Edge;
                    e3->to = nfaBrackets->startState;
                    e3->guard = 'ε';
                    nfaBrackets->acceptingState->nextStates.push_back(e3);

                    if (regularExpression[j+1] == '*')
                    {
                        struct Edge* e4 = new Edge;
                        e4->to = sNewAccepting;
                        e4->guard = 'ε';
                        prevNFA->acceptingState->nextStates.push_back(e4);
                    }
                }
                prevNFA->acceptingState = sNewAccepting;
            }
            else
            {
                struct State* sNew = new State;
                sNew->id = getNextID();


                struct Edge* eNew = new Edge;
                eNew->to = sNew;
                eNew->guard = regularExpression[i];
                prevNFA->acceptingState->nextStates.push_back(eNew);

                if (regularExpression[i+1] == '*' || regularExpression[i+1] == '+')
                {
                    struct State* newAcceptingState = new State;
                    newAcceptingState->id = getNextID();
                    struct Edge* e1 = new Edge;
                    e1->to = newAcceptingState;
                    e1->guard = 'ε';
                    sNew->nextStates.push_back(e1); // SHOULD BE SNEWACCEPTING

                    struct Edge* e2 = new Edge;
                    e2->to = prevNFA->acceptingState;
                    e2->guard = 'ε';
                    newAcceptingState->nextStates.push_back(e2);

                    if (regularExpression[i+1] == '*')
                    {
                        struct Edge* e3 = new Edge;
                        e3->to = newAcceptingState;
                        e3->guard = 'ε';
                        prevNFA->acceptingState->nextStates.push_back(e3);
                    }

                    prevNFA->acceptingState = newAcceptingState;
                    i++;
                }
                else
                {
                    prevNFA->acceptingState = sNew; // SHOULD BE SNEWACCEPTING
                }
            }

        }
        result = prevNFA;
    }
    return result;
};

struct NFA* getNFA(string regularExpression)
{
    return getNFAindexed(regularExpression, 0, regularExpression.length());
};

void showNFA(struct NFA* nfa)
{
    stack<struct State*> statesToVisit;
    set<int> visited;

    visited.insert(nfa->startState->id);
    statesToVisit.push(nfa->startState);
    while (!statesToVisit.empty())
    {
        struct State* current = statesToVisit.top();
        statesToVisit.pop();
        cout << "FROM STATE " << current->id << ", WE CAN VISIT: " << endl;
        vector<struct Edge*> neighbors = current->nextStates;
        for (int i = 0; i < neighbors.size(); i++)
        {
            struct Edge* neighbor = neighbors[i];
            cout << "state: " << neighbor->to->id << ", for input " << neighbor->guard << endl;
            if (visited.count(neighbor->to->id) == 0)
            {
                visited.insert(neighbor->to->id);
                statesToVisit.push(neighbor->to);
            }
        }
        cout << "\n\n";
    }

    cout << "Start State is: " << nfa->startState->id << ", Accepting State is: " << nfa->acceptingState->id << endl;
}

int main()
{
    string regularExpression = "a(a|b)*|c";
    string regularExpression2 = "(a|b(c|d|e|f)*bc)*de|g";
    string regularExpression3 = "ab*c";
    string regularExpression4 = "ab*|cd+|gk";
    string regularExpression5 = "ab";
    string regularExpression6 = "ab*|bc+";

    struct NFA* nfa = getNFA(regularExpression);
    showNFA(nfa);

    return 0;
}


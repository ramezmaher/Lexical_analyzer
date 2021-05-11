#include <set>
#include "NFA.h"
#include "DFA_converter.h"
#include "NFA_maker.h"
#include "DFA.h"
#include <iostream>
#include <fstream>

vector<string> rulesLines;

vector<string> keywords;
vector<char> punctuations;
map<string, string> regularDefinitions;
map<string, string> regularExpressions;

vector<DFA*> automata;
vector<string> keys;

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

string expand(string s)
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

void read(char* filename) {
    string textLine;

    ifstream myfile (filename);
    while (getline (myfile, textLine)) {
        rulesLines.push_back(textLine);
    }
}

void processRulesFile(char* filename) {

    read(filename);

    for (int i = 0; i < rulesLines.size(); i++) {
        string current = rulesLines[i];
        // Keywords
        if (current[0] == '{') {
            int start = 1;
            while (start < current.size() - 1) {
                int ending = start + 1;
                while (current[ending] != ' ' && current[ending] != '}')
                    ending++;
                string found = current.substr(start, ending - start);
                keywords.push_back(found);

                start = ending+1;
            }

            // Punctuation
        } else if (current[0] == '[') {
            for (int j = 1; j < current.size() - 1; j++) {
                if (current[j] == '\\' && current[j-1] != '\\')
                    continue;
                if (current[j] == ' ')
                    continue;
                punctuations.push_back(current[j]);
            }
        }

            // Regular Expression
        else if(current.find(':') != -1){
            int pos = current.find(":");
            string part1 = current.substr(0, pos);
            string part2 = current.substr(pos+1);
            part2 = expand(part2);
            regularExpressions.insert(pair<string, string>(part1, part2));
        }

            // Regular Defintion
        else if (current.find('=') != -1) {
            int pos = current.find("=");
            current = removeShortcuts(current);
            string part1 = current.substr(0, pos - 1);
            string part2 = current.substr(pos + 2);
            part2 = expand(part2);
            regularDefinitions.insert(pair<string, string>(part1, part2));

        }

        else {
            cout << current << " is incorrect input\n";
        }
    }
}

// Remaining function
void simulate(string prog) {

}

int main()
{
    char* filename = "lexical_rules.txt";
    processRulesFile(filename);

    set<char> set = {'a','b','c'}; // Should be removed

    map<string, string>::iterator itr;
    for (itr = regularExpressions.begin(); itr != regularExpressions.end(); itr++) {
        string key = itr->first;
        string regularExpression = itr->second;
        NFA_maker* make = new NFA_maker();
        NFA* nfa = make->getNFA(regularExpression);

        DFA_converter* dfac = new DFA_converter();
        DFA* dfa = dfac->convert_NFA(nfa, set);
        automata.push_back(dfa);
        keys.push_back(key);
    }

    simulate("prog.txt");

    return 0;
}
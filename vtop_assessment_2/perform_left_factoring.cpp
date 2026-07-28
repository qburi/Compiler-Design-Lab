#include <bits/stdc++.h>

using namespace std;

// Helper to find the longest common prefix between two strings
string commonPrefix(const string& s1, const string& s2) {
    int i = 0;
    while (i < s1.length() && i < s2.length() && s1[i] == s2[i]) i++;
    return s1.substr(0, i);
}

void leftFactor(map<string, vector<string>>& grammar) {
    map<string, vector<string>> newGrammar;

    for (auto it = grammar.begin(); it != grammar.end(); ++it) {
        string nt = it->first;
        vector<string> prods = it->second;
        
        if (prods.size() <= 1) {
            newGrammar[nt] = prods;
            continue;
        }

        // Sort to bring strings with common prefixes adjacent
        sort(prods.begin(), prods.end());
        
        string prefix = commonPrefix(prods[0], prods[1]);
        if (prefix.empty()) {
            newGrammar[nt] = prods;
            continue;
        }

        string newNt = nt + "'";
        newGrammar[nt].push_back(prefix + newNt);

        for (const string& prod : prods) {
            if (prod.substr(0, prefix.length()) == prefix) {
                string remainder = prod.substr(prefix.length());
                newGrammar[newNt].push_back(remainder.empty() ? "#" : remainder);
            } else {
                newGrammar[nt].push_back(prod);
            }
        }
    }
    grammar = newGrammar;
}
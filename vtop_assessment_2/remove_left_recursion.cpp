#include <bits/stdc++.h>

using namespace std;

void removeLeftRecursion(map<string, vector<string>>& grammar) {
    map<string, vector<string>> newGrammar;

    for (const auto& [nt, productions] : grammar) {
        vector<string> alpha, beta;

        // Separate productions into alpha (recursive) and beta (non-recursive)
        for (const string& prod : productions) {
            if (prod.length() >= nt.length() && prod.substr(0, nt.length()) == nt) {
                alpha.push_back(prod.substr(nt.length()));
            } else {
                beta.push_back(prod);
            }
        }

        if (alpha.empty()) {
            newGrammar[nt] = productions; // No left recursion
        } else {
            string newNt = nt + "'";
            for (const string& b : beta) {
                newGrammar[nt].push_back(b + newNt);
            }
            if (beta.empty()) {
                newGrammar[nt].push_back(newNt);
            }
            for (const string& a : alpha) {
                newGrammar[newNt].push_back(a + newNt);
            }
            newGrammar[newNt].push_back("#"); // # represents epsilon
        }
    }
    grammar = newGrammar;
}
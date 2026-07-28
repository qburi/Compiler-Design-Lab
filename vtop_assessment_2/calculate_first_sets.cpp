#include <bits/stdc++.h>

using namespace std;

map<char, vector<string>> prod;
map<char, set<char>> firstSets;

void calculateFirst(char nt) {
    if (!firstSets[nt].empty()) return; // Already calculated (memoization)

    for (const string& rhs : prod[nt]) {
        if (rhs == "#") {
            firstSets[nt].insert('#');
        } else if (!isupper(rhs[0])) { // Terminal
            firstSets[nt].insert(rhs[0]);
        } else { // Non-terminal
            for (int i = 0; i < rhs.length(); i++) {
                char nextSym = rhs[i];
                if (!isupper(nextSym)) {
                    firstSets[nt].insert(nextSym);
                    break;
                }
                
                calculateFirst(nextSym);
                bool hasEpsilon = false;
                
                for (char c : firstSets[nextSym]) {
                    if (c == '#') hasEpsilon = true;
                    else firstSets[nt].insert(c);
                }
                
                if (!hasEpsilon) break; 
                if (i == rhs.length() - 1) firstSets[nt].insert('#');
            }
        }
    }
}
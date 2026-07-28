#include <bits/stdc++.h>

using namespace std;

map<char, set<char>> followSets;

void calculateFollow(char startSymbol) {
    // 1. Initialize Follow sets
    for (const auto& pair : prod) {
        followSets[pair.first] = set<char>();
    }
    // Place '$' (end of input) into the start symbol's follow set
    followSets[startSymbol].insert('$');

    bool changed = true;
    
    // 2. Iteratively update Follow sets until no changes occur
    while (changed) {
        changed = false;

        for (const auto& [nt, rhsList] : prod) {
            for (const string& rhs : rhsList) {
                for (int i = 0; i < rhs.length(); i++) {
                    if (!isupper(rhs[i])) continue; // Only process non-terminals
                    
                    char currentNt = rhs[i];
                    int initialSize = followSets[currentNt].size();
                    bool nextContainsEpsilon = true;

                    // Check everything to the right of currentNt
                    for (int j = i + 1; j < rhs.length() && nextContainsEpsilon; j++) {
                        char nextSym = rhs[j];
                        nextContainsEpsilon = false;

                        if (!isupper(nextSym)) {
                            followSets[currentNt].insert(nextSym);
                        } else {
                            for (char f : firstSets[nextSym]) {
                                if (f == '#') nextContainsEpsilon = true;
                                else followSets[currentNt].insert(f);
                            }
                        }
                    }

                    // If we reached the end of the string, or everything right of currentNt can be epsilon
                    if (nextContainsEpsilon) {
                        for (char f : followSets[nt]) {
                            followSets[currentNt].insert(f);
                        }
                    }

                    if (followSets[currentNt].size() > initialSize) {
                        changed = true;
                    }
                }
            }
        }
    }
}
#include <bits/stdc++.h>

using namespace std;

// ==========================================
// 1. Removing Immediate Left Recursion
// ==========================================
void removeLeftRecursion(map<string, vector<string>>& grammar) {
    map<string, vector<string>> newGrammar;

    for (const auto& [nt, productions] : grammar) {
        vector<string> alpha, beta;

        for (const string& prod : productions) {
            if (prod.length() >= nt.length() && prod.substr(0, nt.length()) == nt) {
                alpha.push_back(prod.substr(nt.length()));
            } else {
                beta.push_back(prod);
            }
        }

        if (alpha.empty()) {
            newGrammar[nt] = productions; 
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
            newGrammar[newNt].push_back("#"); 
        }
    }
    grammar = newGrammar;
}

// ==========================================
// 2. Performing Left Factoring
// ==========================================
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

// ==========================================
// 3 & 4. Calculating FIRST and FOLLOW Sets
// ==========================================
map<char, vector<string>> prod;
map<char, set<char>> firstSets;
map<char, set<char>> followSets;

void calculateFirst(char nt) {
    if (!firstSets[nt].empty()) return; 

    for (const string& rhs : prod[nt]) {
        if (rhs == "#") {
            firstSets[nt].insert('#');
        } else if (!isupper(rhs[0])) { 
            firstSets[nt].insert(rhs[0]);
        } else { 
            for (size_t i = 0; i < rhs.length(); i++) {
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

void calculateFollow(char startSymbol) {
    for (const auto& pair : prod) {
        followSets[pair.first] = set<char>();
    }
    followSets[startSymbol].insert('$');

    bool changed = true;
    while (changed) {
        changed = false;

        for (const auto& [nt, rhsList] : prod) {
            for (const string& rhs : rhsList) {
                for (size_t i = 0; i < rhs.length(); i++) {
                    if (!isupper(rhs[i])) continue; 
                    
                    char currentNt = rhs[i];
                    int initialSize = followSets[currentNt].size();
                    bool nextContainsEpsilon = true;

                    for (size_t j = i + 1; j < rhs.length() && nextContainsEpsilon; j++) {
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

// ==========================================
// Utility Functions for Printing
// ==========================================
void printGrammar(const map<string, vector<string>>& grammar) {
    for (const auto& [nt, prods] : grammar) {
        cout << nt << " -> ";
        for (size_t i = 0; i < prods.size(); ++i) {
            cout << prods[i] << (i < prods.size() - 1 ? " | " : "");
        }
        cout << endl;
    }
}

void printSets(const map<char, set<char>>& sets, const string& setName) {
    for (const auto& [nt, elements] : sets) {
        cout << setName << "(" << nt << ") = { ";
        for (auto it = elements.begin(); it != elements.end(); ++it) {
            cout << *it << (next(it) != elements.end() ? ", " : "");
        }
        cout << " }" << endl;
    }
}

// ==========================================
// Main Function & Test Cases
// ==========================================
int main() {
    cout << "---------------------------------------" << endl;
    cout << " 1. TEST CASE: Left Recursion Removal" << endl;
    cout << "---------------------------------------" << endl;
    map<string, vector<string>> lrGrammar = {
        {"E", {"E+T", "T"}},
        {"T", {"T*F", "F"}}
    };
    cout << "Original Grammar:" << endl;
    printGrammar(lrGrammar);
    
    removeLeftRecursion(lrGrammar);
    
    cout << "\nAfter Left Recursion Removal:" << endl;
    printGrammar(lrGrammar);
    cout << "\n\n";


    cout << "---------------------------------------" << endl;
    cout << " 2. TEST CASE: Left Factoring" << endl;
    cout << "---------------------------------------" << endl;
    map<string, vector<string>> lfGrammar = {
        {"S", {"iEtS", "iEtSeS", "a"}},
        {"E", {"b"}}
    };
    cout << "Original Grammar:" << endl;
    printGrammar(lfGrammar);
    
    leftFactor(lfGrammar);
    
    cout << "\nAfter Left Factoring:" << endl;
    printGrammar(lfGrammar);
    cout << "\n\n";


    cout << "---------------------------------------" << endl;
    cout << " 3 & 4. TEST CASE: FIRST and FOLLOW" << endl;
    cout << "---------------------------------------" << endl;
    
    // Q acts as E', R acts as T' for standard expression grammar
    prod = {
        {'E', {"TQ"}},
        {'Q', {"+TQ", "#"}},
        {'T', {"FR"}},
        {'R', {"*FR", "#"}},
        {'F', {"(E)", "i"}}
    };
    
    cout << "Grammar:" << endl;
    for (const auto& [nt, prods] : prod) {
        cout << nt << " -> ";
        for (size_t i = 0; i < prods.size(); ++i) {
            cout << prods[i] << (i < prods.size() - 1 ? " | " : "");
        }
        cout << endl;
    }
    cout << endl;

    // Calculate FIRST for all non-terminals
    for (const auto& pair : prod) {
        calculateFirst(pair.first);
    }
    
    cout << "Calculated FIRST Sets:" << endl;
    printSets(firstSets, "FIRST");
    cout << endl;

    // Calculate FOLLOW starting with Start Symbol 'E'
    calculateFollow('E');
    
    cout << "Calculated FOLLOW Sets:" << endl;
    printSets(followSets, "FOLLOW");
    cout << endl;

    return 0;
}
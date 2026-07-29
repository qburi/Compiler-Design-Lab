#include <bits/stdc++.h>

using namespace std;

void eliminateLeftRecursion(string production) {
    char nonTerminal = production[0];
    string rightSide = production.substr(3); // Skip "A->"
    
    vector<string> alphas, betas;
    stringstream ss(rightSide);
    string token;
    
    // Split productions by '|'
    while (getline(ss, token, '|')) {
        if (token[0] == nonTerminal) {
            alphas.push_back(token.substr(1)); // Extract alpha
        } else {
            betas.push_back(token); // Extract beta
        }
    }
    
    if (alphas.empty()) {
        cout << "No immediate left recursion found." << endl;
        return;
    }
    
    // Generate new productions
    string newNonTerminal = string(1, nonTerminal) + "'";
    
    cout << nonTerminal << " -> ";
    for (size_t i = 0; i < betas.size(); ++i) {
        cout << betas[i] << newNonTerminal;
        if (i != betas.size() - 1) cout << " | ";
    }
    cout << endl;
    
    cout << newNonTerminal << " -> ";
    for (size_t i = 0; i < alphas.size(); ++i) {
        cout << alphas[i] << newNonTerminal << " | ";
    }
    cout << "epsilon" << endl;
}

int main() {
    string grammar = "E->E+T|T";
    cout << "Original: " << grammar << endl;
    eliminateLeftRecursion(grammar);
    return 0;
}
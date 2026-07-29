#include <bits/stdc++.h>

using namespace std;

// Helper to find the longest common prefix between two strings
string getCommonPrefix(string s1, string s2) {
    string prefix = "";
    int n = min(s1.length(), s2.length());
    for (int i = 0; i < n; ++i) {
        if (s1[i] == s2[i]) {
            prefix += s1[i];
        } else {
            break;
        }
    }
    return prefix;
}

void leftFactor(string production) {
    char nonTerminal = production[0];
    string rightSide = production.substr(3);

    vector<string> prods;
    stringstream ss(rightSide);
    string token;

    while (getline(ss, token, '|')) {
        prods.push_back(token);
    }

    if (prods.size() < 2) {
        cout << "Not enough productions for left factoring." << endl;
        return;
    }

    // Find the longest common prefix among all productions
    string prefix = prods[0];
    for (size_t i = 1; i < prods.size(); ++i) {
        prefix = getCommonPrefix(prefix, prods[i]);
    }

    if (prefix.empty()) {
        cout << "No common prefix found. Left factoring not required." << endl;
        return;
    }

    string newNonTerminal = string(1, nonTerminal) + "'";

    // Print the factored out production
    cout << nonTerminal << " -> " << prefix << newNonTerminal << endl;

    // Print the remaining parts
    cout << newNonTerminal << " -> ";
    for (size_t i = 0; i < prods.size(); ++i) {
        string remaining = prods[i].substr(prefix.length());
        if (remaining.empty()) {
            cout << "epsilon";
        } else {
            cout << remaining;
        }
        if (i != prods.size() - 1) cout << " | ";
    }
    cout << endl;
}

int main() {
    string grammar = "S->iEtS|iEtSeS|a";
    cout << "Original: " << grammar << endl;
    leftFactor(grammar);
    return 0;
}
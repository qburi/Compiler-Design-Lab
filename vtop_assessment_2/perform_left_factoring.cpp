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

    // 1. Find the longest prefix shared by at least TWO productions
    string bestPrefix = "";
    for (size_t i = 0; i < prods.size(); ++i) {
        for (size_t j = i + 1; j < prods.size(); ++j) {
            string currentPrefix = getCommonPrefix(prods[i], prods[j]);
            if (currentPrefix.length() > bestPrefix.length()) {
                bestPrefix = currentPrefix;
            }
        }
    }

    if (bestPrefix.empty()) {
        cout << "No common prefix found. Left factoring not required." << endl;
        return;
    }

    string newNonTerminal = string(1, nonTerminal) + "'";

    vector<string> factoredProds;
    vector<string> unchangedProds;

    // 2. Separate productions that share the prefix from those that do not
    for (const string& p : prods) {
        if (p.substr(0, bestPrefix.length()) == bestPrefix) {
            string remaining = p.substr(bestPrefix.length());
            if (remaining.empty()) remaining = "epsilon";
            factoredProds.push_back(remaining);
        } else {
            unchangedProds.push_back(p);
        }
    }

    // 3. Print the modified original production
    cout << nonTerminal << " -> " << bestPrefix << newNonTerminal;
    for (const string& p : unchangedProds) {
        cout << " | " << p;
    }
    cout << endl;

    // 4. Print the new factored production
    cout << newNonTerminal << " -> ";
    for (size_t i = 0; i < factoredProds.size(); ++i) {
        cout << factoredProds[i];
        if (i != factoredProds.size() - 1) cout << " | ";
    }
    cout << endl;
}

int main() {
    string grammar = "S->iEtS|iEtSeS|a";
    cout << "Original: " << grammar << endl;
    leftFactor(grammar);
    return 0;
}
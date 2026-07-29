#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <algorithm>

using namespace std;

class LeftFactorer {
private:
    unordered_map<string, vector<string>> grammar;

    string getCommonPrefix(const string& s1, const string& s2) {
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

    string getNewNonTerminal(string nt) {
        string newNt = nt + "'";
        while (grammar.find(newNt) != grammar.end()) {
            newNt += "'";
        }
        return newNt;
    }

public:
    void addProduction(const string& prodStr) {
        size_t arrowPos = prodStr.find("->");
        if (arrowPos == string::npos) return;

        string nt = prodStr.substr(0, arrowPos);
        string rhs = prodStr.substr(arrowPos + 2);

        stringstream ss(rhs);
        string token;
        vector<string> productions;

        while (getline(ss, token, '|')) {
            productions.push_back(token);
        }
        grammar[nt] = productions;
    }

    void factor() {
        bool changed = true;

        while (changed) {
            changed = false;
            vector<string> nonTerminals;
            for (const auto& pair : grammar) {
                nonTerminals.push_back(pair.first);
            }

            for (const string& nt : nonTerminals) {
                vector<string>& prods = grammar[nt];
                if (prods.size() < 2) continue;

                string bestPrefix = "";

                for (size_t i = 0; i < prods.size(); ++i) {
                    for (size_t j = i + 1; j < prods.size(); ++j) {
                        if (prods[i] == "epsilon" || prods[j] == "epsilon") continue;

                        string currentPrefix = getCommonPrefix(prods[i], prods[j]);
                        if (currentPrefix.length() > bestPrefix.length()) {
                            bestPrefix = currentPrefix;
                        }
                    }
                }

                if (bestPrefix.empty()) continue;

                changed = true;
                string newNt = getNewNonTerminal(nt);
                vector<string> factoredProds;
                vector<string> unchangedProds;

                for (const string& p : prods) {
                    if (p != "epsilon" && p.substr(0, bestPrefix.length()) == bestPrefix) {
                        string remaining = p.substr(bestPrefix.length());
                        if (remaining.empty()) remaining = "epsilon";
                        factoredProds.push_back(remaining);
                    } else {
                        unchangedProds.push_back(p);
                    }
                }

                unchangedProds.push_back(bestPrefix + newNt);
                grammar[nt] = unchangedProds;
                grammar[newNt] = factoredProds;

                break;
            }
        }
    }

    void printGrammar() const {
        for (const auto& pair : grammar) {
            cout << pair.first << " -> ";
            for (size_t i = 0; i < pair.second.size(); ++i) {
                cout << pair.second[i];
                if (i != pair.second.size() - 1) cout << " | ";
            }
            cout << endl;
        }
    }
};

int main() {
    LeftFactorer lf;
    lf.addProduction("S->iEtS|iEtSeS|a|epsilon|Sa|Sb");

    cout << "--- Original Grammar ---" << endl;
    lf.printGrammar();

    lf.factor();

    cout << "\n--- After Left Factoring ---" << endl;
    lf.printGrammar();

    return 0;
}
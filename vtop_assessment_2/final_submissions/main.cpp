#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <algorithm>

using namespace std;

class GrammarProcessor {
public:
    unordered_map<string, vector<string>> grammar;
    bool executed = false;

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

    void eliminateLeftRecursion() {
        vector<string> nonTerminals;
        for (const auto& pair : grammar) {
            nonTerminals.push_back(pair.first);
        }

        for (const string& nt : nonTerminals) {
            vector<string> alphas, betas;

            for (const string& prod : grammar[nt]) {
                if (prod.length() >= nt.length() && prod.substr(0, nt.length()) == nt) {
                    alphas.push_back(prod.substr(nt.length())); // Extract alpha
                } else {
                    betas.push_back(prod); // Extract beta
                }
            }

            if (alphas.empty()) continue;

            string newNt = getNewNonTerminal(nt);
            vector<string> newBetas, newAlphas;

            if (betas.empty()) {
                newBetas.push_back(newNt);
            } else {
                for (const string& beta : betas) {
                    newBetas.push_back((beta == "epsilon" ? "" : beta) + newNt);
                }
            }

            for (const string& alpha : alphas) {
                newAlphas.push_back(alpha + newNt);
            }
            newAlphas.push_back("epsilon");

            grammar[nt] = newBetas;
            grammar[newNt] = newAlphas;

            cout << endl;
            cout << "Intermediate Grammar: " << endl;
            printGrammar();
            cout << endl;
            cout << endl;
        }
    }


    void leftFactor() {
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

                executed = true;
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

                // Update the map
                unchangedProds.push_back(bestPrefix + newNt);
                grammar[nt] = unchangedProds;
                grammar[newNt] = factoredProds;

                cout << endl;
                cout << "Intermediate Grammar: " << endl;
                printGrammar();
                cout << endl;
                cout << endl;

                // Break out of the for-loop to refresh the map iterators
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
    GrammarProcessor gp;

    int n;
    cout << "Enter the number of production rules: " << endl;
    cin >> n;

    for (int i = 0; i < n; i++) {
        cout << "Enter production rule: " << endl;
        string line;
        cin >> line;
        gp.addProduction(line);
    }


    cout << "Original Grammar" << endl;
    gp.printGrammar();


    gp.eliminateLeftRecursion();
    cout << "\n--- After Left Recursion Elimination ---" << endl;
    gp.printGrammar();

    gp.leftFactor();
    cout << endl;
    if (gp.executed) {
        cout << "Left factoring is required." << endl;
    } else cout << "Left factoring is not required." << endl;
    cout << endl;

    cout << "\nAfter Left Factoring" << endl;
    gp.printGrammar();

    return 0;
}
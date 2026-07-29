#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <algorithm>

using namespace std;

class GrammarProcessor {
private:
    // Maps a non-terminal (string) to a list of its productions (vector of strings)
    unordered_map<string, vector<string>> grammar;

    // Helper to find the longest common prefix between two strings
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

    // Helper to generate a unique new non-terminal (e.g., A', A'', etc.)
    string getNewNonTerminal(string nt) {
        string newNt = nt + "'";
        // Ensure the new non-terminal doesn't already exist in the grammar
        while (grammar.find(newNt) != grammar.end()) {
            newNt += "'";
        }
        return newNt;
    }

public:
    // Parses a raw grammar string (e.g., "E->E+T|T") and loads it into the map
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

    // Cascaded Step 1
    void eliminateLeftRecursion() {
        // Copy keys because we will be inserting new elements into the map during iteration
        vector<string> nonTerminals;
        for (const auto& pair : grammar) {
            nonTerminals.push_back(pair.first);
        }

        for (const string& nt : nonTerminals) {
            vector<string> alphas, betas;

            for (const string& prod : grammar[nt]) {
                // FIX: Substring Boundary Check. 
                // Ensures we don't accidentally match "S" with "S'a"
                bool isRecursive = (prod.length() >= nt.length()) && 
                                   (prod.substr(0, nt.length()) == nt) &&
                                   (prod.length() == nt.length() || prod[nt.length()] != '\'');

                if (isRecursive) {
                    alphas.push_back(prod.substr(nt.length())); // Extract alpha
                } else {
                    betas.push_back(prod); // Extract beta
                }
            }

            // Skip if no immediate left recursion is found for this non-terminal
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

            // Update the map
            grammar[nt] = newBetas;
            grammar[newNt] = newAlphas;
        }
    }

    // Cascaded Step 2
    void leftFactor() {
        bool changed = true;

        // Loop ensures that newly factored rules are also checked for further factoring
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

                // Find the longest prefix shared by at least TWO productions
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

                // Separate productions that share the prefix from those that do not
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

                // Break out of the for-loop to refresh the map iterators
                break;
            }
        }
    }

    void printGrammar() const {
        // Collect and sort non-terminals for deterministic lab output
        vector<string> nonTerminals;
        for (const auto& pair : grammar) {
            nonTerminals.push_back(pair.first);
        }
        
        // Sorting ensures base variables like 'E' print before 'E''
        sort(nonTerminals.begin(), nonTerminals.end());

        for (const string& nt : nonTerminals) {
            cout << nt << " -> ";
            const vector<string>& prods = grammar.at(nt);
            for (size_t i = 0; i < prods.size(); ++i) {
                cout << prods[i];
                if (i != prods.size() - 1) cout << " | ";
            }
            cout << endl;
        }
    }
};

int main() {
    GrammarProcessor gp;

    // Load multiple rules into the grammar map
    gp.addProduction("E->E+T|T");
    // Injecting a difficult case right here where 'S' and 'S'' interact
    gp.addProduction("S->iEtS|iEtSeS|a");
    gp.addProduction("S'->S'a|b"); 

    cout << "--- Original Grammar ---" << endl;
    gp.printGrammar();

    // Cascade Operation 1
    cout << "\n--- After Left Recursion Elimination ---" << endl;
    gp.eliminateLeftRecursion();
    gp.printGrammar();

    // Cascade Operation 2
    cout << "\n--- After Left Factoring ---" << endl;
    gp.leftFactor();
    gp.printGrammar();

    return 0;
}
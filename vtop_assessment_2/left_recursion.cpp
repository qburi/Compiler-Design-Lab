#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>

using namespace std;

class LeftRecursionEliminator {
private:
    unordered_map<string, vector<string>> grammar;

    // Helper to generate a unique new non-terminal (e.g., E', E'', etc.)
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

    void eliminate() {
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
    LeftRecursionEliminator lre;
    lre.addProduction("E->E+T|T");

    cout << "--- Original Grammar ---" << endl;
    lre.printGrammar();

    lre.eliminate();

    cout << "\n--- After Left Recursion Elimination ---" << endl;
    lre.printGrammar();

    return 0;
}
// this is the old code

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

void solve() {
    string nt;

    while (cin >> nt) {
        int n;
        if (!(cin >> n)) break;

        vector<string> prods(n);
        for (int i = 0; i < n; i++) {
            cin >> prods[i];
        }

        cout << "Original Grammar\n";
        cout << nt << " -> ";
        for (int i = 0; i < n; i++) {
            cout << prods[i];
            if (i < n - 1) cout << " | ";
        }
        cout << "\n";

        vector<string> alphas;
        vector<string> betas;
        for (const string& p : prods) {
            if (p.rfind(nt, 0) == 0) {
                alphas.push_back(p.substr(nt.length()));
            } else {
                betas.push_back(p);
            }
        }


        bool has_left_recursion = !alphas.empty();
        vector<string> a_prods;
        vector<string> a_prime_prods;

        if (has_left_recursion) {
            for (const string& b : betas) {
                a_prods.push_back(b + nt + "'");
            }
            for (const string& a : alphas) {
                a_prime_prods.push_back(a + nt + "'");
            }
            a_prime_prods.push_back("e");

            cout << "Removing Left Recursion\n";
            cout << nt << " -> ";
            for (size_t i = 0; i < a_prods.size(); i++) {
                cout << a_prods[i];
                if (i < a_prods.size() - 1) cout << " | ";
            }
            cout << "\n";

            cout << nt << "' -> ";
            for (size_t i = 0; i < a_prime_prods.size(); i++) {
                cout << a_prime_prods[i];
                if (i < a_prime_prods.size() - 1) cout << " | ";
            }
            cout << "\n";
        } else {
            a_prods = prods;
        }


        string best_prefix = "";
        for (size_t i = 0; i < a_prods.size(); i++) {
            for (size_t j = i + 1; j < a_prods.size(); j++) {
                string p = "";
                size_t k = 0;
                while (k < a_prods[i].length() && k < a_prods[j].length() && a_prods[i][k] == a_prods[j][k]) {
                    p += a_prods[i][k];
                    k++;
                }
                if (p.length() > best_prefix.length()) {
                    best_prefix = p;
                }
            }
        }

        if (best_prefix.length() > 0) {
            cout << "\nChecking Left Factoring Common Prefix Found : " << best_prefix << "\n";
            cout << "\nAfter Left Factoring\n\n";

            vector<string> factored_rem;
            vector<string> unfactored;

            for (const string& p : a_prods) {
                if (p.rfind(best_prefix, 0) == 0) {
                    string rem = p.substr(best_prefix.length());
                    if (rem == "") rem = "e";
                    factored_rem.push_back(rem);
                } else {
                    unfactored.push_back(p);
                }
            }

            cout << nt << " -> " << best_prefix << "X";
            for (const string& u : unfactored) {
                cout << " | " << u;
            }
            cout << "\n";

            cout << "X -> ";
            for (size_t i = 0; i < factored_rem.size(); i++) {
                cout << factored_rem[i];
                if (i < factored_rem.size() - 1) cout << " | ";
            }
            cout << "\n";
        } else {
            cout << "\nChecking Left Factoring\n";
            cout << "\nAfter Left Factoring\n\n";

            cout << nt << " -> ";
            for (size_t i = 0; i < a_prods.size(); i++) {
                cout << a_prods[i];
                if (i < a_prods.size() - 1) cout << " | ";
            }
            cout << "\n";
        }

        if (has_left_recursion) {
            cout << nt << "' -> ";
            for (size_t i = 0; i < a_prime_prods.size(); i++) {
                cout << a_prime_prods[i];
                if (i < a_prime_prods.size() - 1) cout << " | ";
            }
            cout << "\n";
        }
    }
}

int main() {

    solve();
    return 0;
}
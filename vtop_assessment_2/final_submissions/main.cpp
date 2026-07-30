#include <bits/stdc++.h>

using namespace std;

string get_lcp(const string& a, const string& b) {
    int i = 0;
    while (i < a.length() && i < b.length() && a[i] == b[i]) i++;
    return a.substr(0, i);
}

int main() {
    string nt;
    if (!(cin >> nt)) return 0;

    int n;
    cin >> n;

    vector<string> prods(n);
    for (int i = 0; i < n; i++) cin >> prods[i];

    cout << "Original Grammar\n";
    cout << nt << " -> ";
    for (int i = 0; i < n; i++) {
        cout << prods[i] << (i == n - 1 ? "" : " | ");
    }
    cout << "\n";

    vector<string> alphas, betas;
    for (string p : prods) {
        if (p.length() >= nt.length() && p.substr(0, nt.length()) == nt) {
            alphas.push_back(p.substr(nt.length()));
        } else {
            betas.push_back(p);
        }
    }

    if (alphas.empty()) return 0;

    cout << "Removing Left Recursion\n";
    string nt_prime = nt + "'";
    vector<string> new_nt_prods;

    cout << nt << " -> ";
    for (int i = 0; i < betas.size(); i++) {
        string p = betas[i] + nt_prime;
        new_nt_prods.push_back(p);
        cout << p << (i == betas.size() - 1 ? "" : " | ");
    }
    cout << "\n";

    vector<string> nt_prime_prods;
    cout << nt_prime << " -> ";
    for (int i = 0; i < alphas.size(); i++) {
        string p = alphas[i] + nt_prime;
        nt_prime_prods.push_back(p);
        cout << p << " | ";
    }
    nt_prime_prods.push_back("e");
    cout << "e\n";

    string best_prefix = "";
    if (new_nt_prods.size() >= 2) {
        best_prefix = get_lcp(new_nt_prods[0], new_nt_prods[1]);
        for (int i = 2; i < new_nt_prods.size(); i++) {
            best_prefix = get_lcp(best_prefix, new_nt_prods[i]);
        }
    }

    if (best_prefix.length() > 0) {
        cout << "\nChecking Left Factoring Common Prefix Found : " << best_prefix << "\n";
        cout << "\nAfter Left Factoring\n\n";

        cout << nt << " -> " << best_prefix << "X\n";
        cout << "X -> ";
        for (int i = 0; i < new_nt_prods.size(); i++) {
            string suffix = new_nt_prods[i].substr(best_prefix.length());
            if (suffix == "") suffix = "e";
            cout << suffix << (i == new_nt_prods.size() - 1 ? "" : " | ");
        }
        cout << "\n";

        cout << nt_prime << " -> ";
        for (int i = 0; i < nt_prime_prods.size(); i++) {
            cout << nt_prime_prods[i] << (i == nt_prime_prods.size() - 1 ? "" : " | ");
        }
        cout << "\n";
    } else {
        cout << "\nChecking Left Factoring\n";
        cout << "Left factoring not required\n";
    }

    return 0;
}
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>

using namespace std;

class GrammarAnalyzer {
public:
    string start_symbol;
    string epsilon_sym = "ε";
    string eof_sym = "$";

    map<string, vector<vector<string>>> productions;
    set<string> non_terminals;
    set<string> terminals;

    map<string, set<string>> first;
    map<string, set<string>> follow;

    GrammarAnalyzer(string start) : start_symbol(start) {}

    void add_production(string lhs, vector<string> rhs) {
        productions[lhs].push_back(rhs);
        non_terminals.insert(lhs);
    }

    void extract_terminals() {
        for (const auto& [lhs, bodies] : productions) {
            for (const auto& body : bodies) {
                for (const auto& symbol : body) {
                    if (non_terminals.find(symbol) == non_terminals.end() && symbol != epsilon_sym) {
                        terminals.insert(symbol);
                        // Terminal FIRST set is just itself
                        first[symbol].insert(symbol);
                    }
                }
            }
        }
        first[epsilon_sym].insert(epsilon_sym);
    }

    void compute_first() {
        extract_terminals();
        bool changed = true;

        while (changed) {
            changed = false;
            for (const auto& [head, bodies] : productions) {
                for (const auto& body : bodies) {
                    size_t i = 0;
                    for (; i < body.size(); ++i) {
                        string symbol = body[i];
                        bool has_epsilon = false;

                        // Add FIRST(symbol) to FIRST(head), minus epsilon
                        for (const string& f : first[symbol]) {
                            if (f != epsilon_sym) {
                                // .second is true if a new element was actually inserted
                                if (first[head].insert(f).second) {
                                    changed = true;
                                }
                            } else {
                                has_epsilon = true;
                            }
                        }

                        // Stop lookahead if current symbol can't derive epsilon
                        if (!has_epsilon) break;
                    }

                    // If everything derived epsilon, add epsilon to FIRST(head)
                    if (i == body.size() && first[head].find(epsilon_sym) == first[head].end()) {
                        first[head].insert(epsilon_sym);
                        changed = true;
                    }
                }
            }
        }
    }

    void compute_follow() {
        follow[start_symbol].insert(eof_sym);
        bool changed = true;

        while (changed) {
            changed = false;
            for (const auto& [head, bodies] : productions) {
                for (const auto& body : bodies) {
                    for (size_t i = 0; i < body.size(); ++i) {
                        string symbol = body[i];

                        // Skip terminals
                        if (non_terminals.find(symbol) == non_terminals.end()) continue;

                        bool can_derive_empty = true;
                        set<string> next_first_set;

                        // Lookahead for A -> αBβ
                        for (size_t j = i + 1; j < body.size(); ++j) {
                            string next_symbol = body[j];
                            bool next_has_epsilon = false;

                            for (const string& f : first[next_symbol]) {
                                if (f != epsilon_sym) {
                                    next_first_set.insert(f);
                                } else {
                                    next_has_epsilon = true;
                                }
                            }

                            if (!next_has_epsilon) {
                                can_derive_empty = false;
                                break;
                            }
                        }

                        // Rule 2: Add FIRST(β) to FOLLOW(B)
                        for (const string& f : next_first_set) {
                            if (follow[symbol].insert(f).second) {
                                changed = true;
                            }
                        }

                        // Rule 3: If β -> ε, add FOLLOW(A) to FOLLOW(B)
                        if (can_derive_empty) {
                            for (const string& f : follow[head]) {
                                if (follow[symbol].insert(f).second) {
                                    changed = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void print_sets() {
        cout << "FIRST Sets:\n";
        for (const string& nt : non_terminals) {
            cout << "  FIRST(" << nt << ") = { ";
            for (const string& s : first[nt]) cout << s << " ";
            cout << "}\n";
        }

        cout << "\nFOLLOW Sets:\n";
        for (const string& nt : non_terminals) {
            cout << "  FOLLOW(" << nt << ") = { ";
            for (const string& s : follow[nt]) cout << s << " ";
            cout << "}\n";
        }
    }
};

int main() {
    GrammarAnalyzer g("E");

    g.add_production("E",  {"T", "E_prime"});
    g.add_production("E_prime", {"+", "T", "E_prime"});
    g.add_production("E_prime", {"ε"});
    g.add_production("T",  {"F", "T_prime"});
    g.add_production("T_prime", {"*", "F", "T_prime"});
    g.add_production("T_prime", {"ε"});
    g.add_production("F",  {"(", "E", ")"});
    g.add_production("F",  {"id"});

    g.compute_first();
    g.compute_follow();

    g.print_sets();

    return 0;
}
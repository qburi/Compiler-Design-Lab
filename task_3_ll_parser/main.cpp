#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <stack>
#include <iomanip>

using namespace std;

map<char, vector<string>> grammar;
map<char, set<char>> first_set;
map<char, set<char>> follow_set;
map<pair<char, char>, string> parse_table;
char start_symbol;

bool isNonTerminal(char c) {
    return (c >= 'A' && c <= 'Z');
}

// Function to compute the FIRST set of a single symbol or string
set<char> computeFirst(string s) {
    set<char> f;
    if (s.empty()) return f;

    char first_char = s[0];

    // If it's a terminal or epsilon, add it and return
    if (!isNonTerminal(first_char)) {
        f.insert(first_char);
        return f;
    }

    // If it's a non-terminal, look at its productions
    for (const string& prod : grammar[first_char]) {
        if (prod[0] == first_char) continue; // Basic safeguard against left recursion

        if (!isNonTerminal(prod[0])) {
            f.insert(prod[0]);
        } else {
            bool contains_epsilon = true;
            for (char c : prod) {
                if (!isNonTerminal(c)) {
                    f.insert(c);
                    contains_epsilon = false;
                    break;
                }
                set<char> next_first = computeFirst(string(1, c));
                for (char res : next_first) {
                    if (res != '#') f.insert(res);
                }
                if (next_first.find('#') == next_first.end()) {
                    contains_epsilon = false;
                    break;
                }
            }
            if (contains_epsilon) f.insert('#');
        }
    }
    return f;
}

void buildFirstSets() {
    for (auto const& [nt, prods] : grammar) {
        first_set[nt] = computeFirst(string(1, nt));
    }
}

void buildFollowSets() {
    // Initialize Follow sets
    for (auto const& [nt, prods] : grammar) {
        if (nt == start_symbol) follow_set[nt].insert('$');
    }

    bool changed = true;
    while (changed) {
        changed = false;
        for (auto const& [nt, prods] : grammar) {
            for (const string& prod : prods) {
                for (int i = 0; i < prod.length(); i++) {
                    if (isNonTerminal(prod[i])) {
                        char current_nt = prod[i];
                        int original_size = follow_set[current_nt].size();

                        if (i + 1 < prod.length()) {
                            // There is a symbol after the non-terminal
                            string remaining = prod.substr(i + 1);
                            set<char> next_first = computeFirst(remaining);

                            for (char c : next_first) {
                                if (c != '#') follow_set[current_nt].insert(c);
                            }
                            // If remaining can derive epsilon, add Follow of LHS
                            if (next_first.find('#') != next_first.end()) {
                                for (char c : follow_set[nt]) {
                                    follow_set[current_nt].insert(c);
                                }
                            }
                        } else {
                            // Non-terminal is at the end of production, add Follow of LHS
                            for (char c : follow_set[nt]) {
                                follow_set[current_nt].insert(c);
                            }
                        }

                        if (follow_set[current_nt].size() > original_size) {
                            changed = true;
                        }
                    }
                }
            }
        }
    }
}

void buildParseTable() {
    for (auto const& [nt, prods] : grammar) {
        for (const string& prod : prods) {
            set<char> first_alpha = computeFirst(prod);

            for (char terminal : first_alpha) {
                if (terminal != '#') {
                    parse_table[{nt, terminal}] = prod;
                }
            }

            if (first_alpha.find('#') != first_alpha.end()) {
                for (char terminal : follow_set[nt]) {
                    parse_table[{nt, terminal}] = prod;
                }
            }
        }
    }
}

void parseString(string input) {
    input += "$";
    stack<char> st;
    st.push('$');
    st.push(start_symbol);

    int ptr = 0;
    cout << "\n--- Parsing Steps ---\n";

    while (!st.empty()) {
        char top = st.top();
        char current_char = input[ptr];

        if (top == '$' && current_char == '$') {
            cout << "String Accepted!\n";
            return;
        }

        if (top == current_char) {
            cout << "Matched: " << top << "\n";
            st.pop();
            ptr++;
        } else if (isNonTerminal(top)) {
            if (parse_table.find({top, current_char}) != parse_table.end()) {
                string prod = parse_table[{top, current_char}];
                cout << top << " -> " << prod << "\n";
                st.pop();

                // Push production to stack in reverse order
                if (prod != "#") {
                    for (int i = prod.length() - 1; i >= 0; i--) {
                        st.push(prod[i]);
                    }
                }
            } else {
                cout << "Error: No table entry for [" << top << ", " << current_char << "]\n";
                cout << "String Rejected!\n";
                return;
            }
        } else {
            cout << "Error: Terminal mismatch (" << top << " != " << current_char << ")\n";
            cout << "String Rejected!\n";
            return;
        }
    }
}

int main() {
    // Example Grammar:
    // E -> TR
    // R -> +TR | #
    // T -> FY
    // Y -> *FY | #
    // F -> (E) | i

    start_symbol = 'E';
    grammar['E'] = {"TR"};
    grammar['R'] = {"+TR", "#"};
    grammar['T'] = {"FY"};
    grammar['Y'] = {"*FY", "#"};
    grammar['F'] = {"(E)", "i"};

    buildFirstSets();
    buildFollowSets();
    buildParseTable();

    // Print First Sets
    cout << "--- FIRST SETS ---\n";
    for (auto const& [nt, firsts] : first_set) {
        cout << nt << ": { ";
        for (char c : firsts) cout << c << " ";
        cout << "}\n";
    }

    // Print Follow Sets
    cout << "--- FOLLOW SETS ---\n";
    for (auto const& [nt, follows] : follow_set) {
        cout << nt << ": { ";
        for (char c : follows) cout << c << " ";
        cout << "}\n";
    }

    // Test the parser with an input string
    string input_string = "i+i*i";
    cout << "\nParsing input: " << input_string << "\n";
    parseString(input_string);

    return 0;
}
/*
 * FIRST and FOLLOW set calculator for a context-free grammar.
 * ------------------------------------------------------------
 * Everything lives inside a single class (Grammar) so the flow is easy
 * to follow: read grammar -> compute FIRST -> compute FOLLOW -> print.
 *
 * INPUT FORMAT
 * ------------
 * Line 1: N              -> number of productions
 * Next N lines           -> productions written as:
 *                              A -> alpha1 | alpha2 | ... | alphaK
 *                            where each alpha is a space-separated
 *                            sequence of grammar symbols.
 *                            Use '#' to mean epsilon (the empty string).
 *
 * The LHS of the FIRST production read is taken as the start symbol.
 *
 * EXAMPLE
 * -------
 * 4
 * E -> T E'
 * E' -> + T E' | #
 * T -> F T'
 * T' -> * F T' | #
 *
 * RULES USED TO CLASSIFY SYMBOLS
 * -------------------------------
 * - Any symbol that ever appears on the LEFT of "->" is a non-terminal.
 * - Any other symbol seen on a RHS (other than '#') is a terminal.
 *
 * WHY THIS IS "ROBUST"
 * ---------------------
 * - Works with multi-character symbol names (e.g. "E'", "stmt_list"),
 *   not just single letters.
 * - Uses the classic *iterative fixed-point* algorithm, not recursion,
 *   so it terminates correctly even on left-recursive, indirectly
 *   left-recursive, or cyclic (A -> B, B -> A) grammars -- no infinite
 *   recursion, no stack overflow.
 * - Handles epsilon-productions and nullable non-terminals correctly
 *   in both FIRST and FOLLOW computation.
 * - Handles multiple non-terminals on the RHS with nullable prefixes
 *   (e.g. FOLLOW propagation through A -> B C D where B, C are
 *   nullable).
 */

#include <bits/stdc++.h>
using namespace std;

class Grammar {
private:
    static const string EPSILON;   // internal symbol for epsilon, "#"
    static const string END_MARK;  // end-of-input marker, "$"

    vector<string> productionOrder;                   // non-terminals in first-seen order (start symbol first)
    map<string, vector<vector<string>>> productions;  // LHS -> list of RHS symbol sequences
    set<string> nonTerminals;
    set<string> terminals;
    string startSymbol;

    map<string, set<string>> firstSet;
    map<string, set<string>> followSet;

    static vector<string> splitWS(const string& s) {
        vector<string> tokens;
        stringstream ss(s);
        string tok;
        while (ss >> tok) tokens.push_back(tok);
        return tokens;
    }

    static string trim(const string& s) {
        size_t a = s.find_first_not_of(" \t\r\n");
        if (a == string::npos) return "";
        size_t b = s.find_last_not_of(" \t\r\n");
        return s.substr(a, b - a + 1);
    }

    bool isNonTerminal(const string& sym) const {
        return nonTerminals.count(sym) > 0;
    }

    // FIRST of the symbol sequence seq[startIdx..end], using whatever
    // FIRST sets are currently known. Safe to call mid-fixed-point
    // (used by computeFirst) and again once FIRST is final (used by
    // computeFollow).
    set<string> firstOfSequence(const vector<string>& seq, size_t startIdx) const {
        set<string> result;
        bool allNullableSoFar = true;
        for (size_t i = startIdx; i < seq.size(); i++) {
            if (!allNullableSoFar) break;
            const string& sym = seq[i];
            if (sym == EPSILON) continue; // an explicit epsilon symbol contributes nothing itself
            if (!isNonTerminal(sym)) {
                result.insert(sym);        // terminal: stop here
                allNullableSoFar = false;
            } else {
                const set<string>& f = firstSet.at(sym);
                for (const string& s : f) if (s != EPSILON) result.insert(s);
                if (!f.count(EPSILON)) allNullableSoFar = false;
            }
        }
        if (allNullableSoFar) result.insert(EPSILON); // entire (sub)sequence can vanish
        return result;
    }

public:
    void inputFromStream(istream& in) {
        int n;
        in >> n;
        in.ignore(); // consume rest of the line with n

        for (int i = 0; i < n; i++) {
            string line;
            if (!getline(in, line)) break;
            line = trim(line);
            if (line.empty()) { i--; continue; } // ignore stray blank lines, don't miscount

            size_t arrowPos = line.find("->");
            if (arrowPos == string::npos) {
                cerr << "Warning: skipping malformed production (no '->'): " << line << "\n";
                i--; continue;
            }
            string lhs = trim(line.substr(0, arrowPos));
            string rhsPart = line.substr(arrowPos + 2);

            if (!nonTerminals.count(lhs)) {
                nonTerminals.insert(lhs);
                productionOrder.push_back(lhs);
                if (startSymbol.empty()) startSymbol = lhs;
            }

            // split alternatives on '|'
            stringstream alts(rhsPart);
            string alt;
            while (getline(alts, alt, '|')) {
                vector<string> symbols = splitWS(alt);
                if (symbols.empty()) symbols.push_back(EPSILON);
                productions[lhs].push_back(symbols);
            }
        }

        // Second pass: any RHS symbol that isn't a known non-terminal
        // and isn't epsilon must be a terminal.
        for (auto& entry : productions) {
            for (auto& rhs : entry.second) {
                for (auto& sym : rhs) {
                    if (sym != EPSILON && !nonTerminals.count(sym)) {
                        terminals.insert(sym);
                    }
                }
            }
        }

        for (const string& nt : nonTerminals) {
            firstSet[nt];   // default-construct empty sets
            followSet[nt];
        }
    }

    void computeFirst() {
        bool changed = true;
        while (changed) {
            changed = false;
            for (const string& A : productionOrder) {
                for (const vector<string>& rhs : productions[A]) {
                    set<string> f = firstOfSequence(rhs, 0);
                    for (const string& s : f) {
                        if (firstSet[A].insert(s).second) changed = true;
                    }
                }
            }
        }
    }

    void computeFollow() {
        if (startSymbol.empty()) return;
        followSet[startSymbol].insert(END_MARK);

        bool changed = true;
        while (changed) {
            changed = false;
            for (const string& A : productionOrder) {
                for (const vector<string>& rhs : productions[A]) {
                    for (size_t i = 0; i < rhs.size(); i++) {
                        const string& B = rhs[i];
                        if (!isNonTerminal(B)) continue;

                        set<string> firstBeta = firstOfSequence(rhs, i + 1);
                        for (const string& s : firstBeta) {
                            if (s != EPSILON && followSet[B].insert(s).second) changed = true;
                        }
                        if (firstBeta.count(EPSILON)) {
                            for (const string& s : followSet[A]) {
                                if (followSet[B].insert(s).second) changed = true;
                            }
                        }
                    }
                }
            }
        }
    }

    void printGrammarSummary() const {
        cout << "Start symbol : " << startSymbol << "\n";
        cout << "Non-terminals: ";
        for (const string& nt : productionOrder) cout << nt << ' ';
        cout << "\nTerminals    : ";
        for (const string& t : terminals) cout << t << ' ';
        cout << "\n\n";
    }

    void printFirst() const {
        cout << "FIRST sets\n----------\n";
        for (const string& A : productionOrder) {
            cout << "FIRST(" << A << ") = { ";
            bool first = true;
            for (const string& s : firstSet.at(A)) {
                if (!first) cout << ", ";
                cout << (s == EPSILON ? "eps" : s);
                first = false;
            }
            cout << " }\n";
        }
        cout << "\n";
    }

    void printFollow() const {
        cout << "FOLLOW sets\n-----------\n";
        for (const string& A : productionOrder) {
            cout << "FOLLOW(" << A << ") = { ";
            bool first = true;
            for (const string& s : followSet.at(A)) {
                if (!first) cout << ", ";
                cout << s;
                first = false;
            }
            cout << " }\n";
        }
        cout << "\n";
    }

    void run(istream& in = cin) {
        inputFromStream(in);
        computeFirst();
        computeFollow();
        printGrammarSummary();
        printFirst();
        printFollow();
    }
};

const string Grammar::EPSILON = "#";
const string Grammar::END_MARK = "$";

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "== FIRST & FOLLOW calculator ==\n";
    cout << "Enter number of productions, then each production as:\n";
    cout << "  A -> alpha1 | alpha2 | ...   (use # for epsilon)\n\n";

    Grammar g;
    g.run(cin);

    return 0;
}
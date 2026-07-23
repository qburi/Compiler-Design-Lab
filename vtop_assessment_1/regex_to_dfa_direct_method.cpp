#include <bits/stdc++.h>

using namespace std;

// Node structure
struct Node {
    char val;
    int pos;
    bool nullable;
    set<int> firstpos;
    set<int> lastpos;
    Node* left;
    Node* right;
    string expr; // Used to print the expression in the table
};

// Global variables for position tracking and mapping
map<int, char> leafPosMap;
vector<set<int>> followpos(100);
int posCounter = 1;

// function to determine operator precedence for postfix conversion
int precedence(char c) {
    if (c == '*') return 3;
    if (c == '.') return 2;
    if (c == '|') return 1;
    return 0;
}

// Convert Infix Regular Expression to Postfix
string infixToPostfix(string s) {
    string postfix = "";
    stack<char> st;
    for (char c : s) {
        if (isalpha(c) || c == '#') {
            postfix += c;
        } else if (c == '(') {
            st.push('(');
        } else if (c == ')') {
            while (!st.empty() && st.top() != '(') {
                postfix += st.top();
                st.pop();
            }
            if (!st.empty()) st.pop();
        } else {
            while (!st.empty() && precedence(st.top()) >= precedence(c)) {
                postfix += st.top();
                st.pop();
            }
            st.push(c);
        }
    }
    while (!st.empty()) {
        postfix += st.top();
        st.pop();
    }
    return postfix;
}

// Build the Syntax Tree from the Postfix expression
Node* buildTree(string postfix) {
    stack<Node*> st;
    for (char c : postfix) {
        if (isalpha(c) || c == '#') {
            Node* n = new Node();
            n->val = c;
            n->pos = posCounter;
            leafPosMap[posCounter] = c;
            posCounter++;
            n->nullable = false;
            n->firstpos.insert(n->pos);
            n->lastpos.insert(n->pos);
            n->left = n->right = nullptr;
            n->expr = string(1, c);
            st.push(n);
        } else if (c == '*') {
            Node* n = new Node();
            n->val = '*';
            n->pos = -1;
            n->left = st.top(); st.pop();
            n->right = nullptr;
            n->nullable = true;
            n->firstpos = n->left->firstpos;
            n->lastpos = n->left->lastpos;
            
            // Format string appropriately based on operand
            if (n->left->val == '|' || n->left->val == '.') n->expr = n->left->expr + "*";
            else n->expr = n->left->expr + "*";
            
            st.push(n);
        } else if (c == '|' || c == '.') {
            Node* n = new Node();
            n->val = c;
            n->pos = -1;
            n->right = st.top(); st.pop();
            n->left = st.top(); st.pop();

            if (c == '|') {
                n->nullable = n->left->nullable || n->right->nullable;
                set_union(n->left->firstpos.begin(), n->left->firstpos.end(), 
                          n->right->firstpos.begin(), n->right->firstpos.end(), 
                          inserter(n->firstpos, n->firstpos.begin()));
                set_union(n->left->lastpos.begin(), n->left->lastpos.end(), 
                          n->right->lastpos.begin(), n->right->lastpos.end(), 
                          inserter(n->lastpos, n->lastpos.begin()));
                n->expr = "(" + n->left->expr + "|" + n->right->expr + ")";
            } else { // '.' concatenation
                n->nullable = n->left->nullable && n->right->nullable;
                if (n->left->nullable) {
                    set_union(n->left->firstpos.begin(), n->left->firstpos.end(), 
                              n->right->firstpos.begin(), n->right->firstpos.end(), 
                              inserter(n->firstpos, n->firstpos.begin()));
                } else {
                    n->firstpos = n->left->firstpos;
                }
                if (n->right->nullable) {
                    set_union(n->left->lastpos.begin(), n->left->lastpos.end(), 
                              n->right->lastpos.begin(), n->right->lastpos.end(), 
                              inserter(n->lastpos, n->lastpos.begin()));
                } else {
                    n->lastpos = n->right->lastpos;
                }
                n->expr = n->left->expr + "." + n->right->expr;
            }
            st.push(n);
        }
    }
    return st.top();
}

// Compute Followpos recursively
void computeFollowpos(Node* root) {
    if (!root) return;
    computeFollowpos(root->left);
    computeFollowpos(root->right);

    if (root->val == '.') {
        for (int i : root->left->lastpos) {
            for (int j : root->right->firstpos) {
                followpos[i].insert(j);
            }
        }
    } else if (root->val == '*') {
        for (int i : root->lastpos) {
            for (int j : root->firstpos) {
                followpos[i].insert(j);
            }
        }
    }
}

// Helper formatting function for sets
string formatSet(const set<int>& s) {
    string res = "{";
    for (auto it = s.begin(); it != s.end(); ++it) {
        if (it != s.begin()) res += ",";
        res += to_string(*it);
    }
    res += "}";
    return res;
}

// Print Nullable, Firstpos, Lastpos via Post-Order Traversal
void printTable(Node* root) {
    if (!root) return;
    printTable(root->left);
    printTable(root->right);
    
    string nullStr = root->nullable ? "True" : "False";
    cout << left << setw(20) << root->expr
         << setw(10) << nullStr
         << setw(15) << formatSet(root->firstpos)
         << setw(15) << formatSet(root->lastpos) << "\n";
}

int main() {
    cout << "Enter Regular Expression:\n";
    string re;
    cin >> re;

    // 1. Augment the regular expression
    string augmented = re + ".#";
    cout << "\nAugmented RE:\n\n" << augmented << "\n\n";

    // 2. Construct Syntax Tree
    string postfix = infixToPostfix(augmented);
    Node* root = buildTree(postfix);

    // Print Leaf Positions
    cout << "Leaf Positions\n\n";
    for (auto const& [pos, sym] : leafPosMap) {
        cout << pos << " -> " << sym << "\n\n";
    }

    // 3. Compute Nullable, Firstpos, Lastpos
    cout << "Nullable Firstpos Lastpos\n";
    cout << string(60, '-') << "\n";
    printTable(root);
    cout << "\n";

    // Compute and Print Followpos
    computeFollowpos(root);
    cout << "Followpos\n\n";
    for (int i = 1; i < posCounter; ++i) {
        cout << "Position " << i << " : " << formatSet(followpos[i]) << "\n\n";
    }

    // 4. Generate DFA Transition Table
    set<char> alphabet;
    for (auto const& [pos, sym] : leafPosMap) {
        if (sym != '#') alphabet.insert(sym);
    }

    map<set<int>, char> stateName;
    vector<set<int>> dfaStates;
    map<pair<char, char>, char> dfaTransitions;
    char nextName = 'A';

    set<int> startState = root->firstpos;
    stateName[startState] = nextName++;
    dfaStates.push_back(startState);

    int p = 0;
    while (p < dfaStates.size()) {
        set<int> current = dfaStates[p++];
        char cName = stateName[current];

        for (char sym : alphabet) {
            set<int> nextState;
            for (int pos : current) {
                if (leafPosMap[pos] == sym) {
                    for (int f : followpos[pos]) {
                        nextState.insert(f);
                    }
                }
            }
            if (!nextState.empty()) {
                if (stateName.find(nextState) == stateName.end()) {
                    stateName[nextState] = nextName++;
                    dfaStates.push_back(nextState);
                }
                dfaTransitions[{cName, sym}] = stateName[nextState];
            }
        }
    }

    cout << "DFA Transition Table\n\n";
    cout << left << setw(20) << "State";
    for (char c : alphabet) {
        cout << setw(10) << c;
    }
    cout << "\n";
    cout << string(20 + alphabet.size() * 10, '-') << "\n\n";

    for (const auto& state : dfaStates) {
        char name = stateName[state];
        string stateLabel = string(1, name) + formatSet(state);
        cout << left << setw(20) << stateLabel;
        for (char c : alphabet) {
            if (dfaTransitions.count({name, c})) {
                cout << setw(10) << dfaTransitions[{name, c}];
            } else {
                cout << setw(10) << "-";
            }
        }
        cout << "\n\n";
    }

    // Identify Final States (any state containing the position of '#')
    cout << "Final State(s): ";
    bool first = true;
    for (const auto& state : dfaStates) {
        if (state.count(posCounter - 1)) { // posCounter - 1 is the '#' position
            if (!first) cout << ", ";
            cout << stateName[state];
            first = false;
        }
    }
    cout << "\n";

    return 0;
}
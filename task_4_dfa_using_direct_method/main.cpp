#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <queue>
#include <algorithm>

using namespace std;

// Node structure for the Syntax Tree
struct Node {
    char op;
    int pos; // -1 for internal nodes, >0 for leaf nodes
    bool nullable;
    set<int> firstpos;
    set<int> lastpos;
    Node* left;
    Node* right;

    Node(char o, int p = -1) : op(o), pos(p), nullable(false), left(nullptr), right(nullptr) {}
};

class DirectDFA {
private:
    map<int, set<int>> followpos;
    map<int, char> posToChar;
    int posCounter = 1;
    set<char> alphabet;

    // Helper to merge two sets
    set<int> mergeSets(const set<int>& s1, const set<int>& s2) {
        set<int> res = s1;
        res.insert(s2.begin(), s2.end());
        return res;
    }

public:
    Node* buildSyntaxTree(const string& postfix) {
        stack<Node*> st;

        for (char ch : postfix) {
            if (ch == '|' || ch == '.') {
                Node* right = st.top(); st.pop();
                Node* left = st.top(); st.pop();
                Node* parent = new Node(ch);
                parent->left = left;
                parent->right = right;
                st.push(parent);
            } else if (ch == '*') {
                Node* child = st.top(); st.pop();
                Node* parent = new Node(ch);
                parent->left = child;
                st.push(parent);
            } else {
                // Leaf node (operand)
                Node* leaf = new Node(ch, posCounter++);
                posToChar[leaf->pos] = ch;
                if (ch != '#') alphabet.insert(ch); // Track input alphabet
                st.push(leaf);
            }
        }
        return st.top();
    }

    void computeFunctions(Node* root) {
        if (!root) return;

        computeFunctions(root->left);
        computeFunctions(root->right);

        // Leaf Node
        if (root->left == nullptr && root->right == nullptr) {
            if (root->op == 'e') { // Epsilon
                root->nullable = true;
            } else {
                root->nullable = false;
                root->firstpos.insert(root->pos);
                root->lastpos.insert(root->pos);
            }
        } 
        // OR Operator (|)
        else if (root->op == '|') {
            root->nullable = root->left->nullable || root->right->nullable;
            root->firstpos = mergeSets(root->left->firstpos, root->right->firstpos);
            root->lastpos = mergeSets(root->left->lastpos, root->right->lastpos);
        } 
        // CONCAT Operator (.)
        else if (root->op == '.') {
            root->nullable = root->left->nullable && root->right->nullable;
            
            root->firstpos = root->left->nullable ? 
                mergeSets(root->left->firstpos, root->right->firstpos) : root->left->firstpos;
            
            root->lastpos = root->right->nullable ? 
                mergeSets(root->left->lastpos, root->right->lastpos) : root->right->lastpos;

            // Followpos computation for CONCAT
            for (int i : root->left->lastpos) {
                followpos[i] = mergeSets(followpos[i], root->right->firstpos);
            }
        } 
        // KLEENE STAR (*)
        else if (root->op == '*') {
            root->nullable = true;
            root->firstpos = root->left->firstpos;
            root->lastpos = root->left->lastpos;

            // Followpos computation for STAR
            for (int i : root->lastpos) {
                followpos[i] = mergeSets(followpos[i], root->firstpos);
            }
        }
    }

    void constructDFA(Node* root) {
        map<set<int>, char> dfaStates;
        map<pair<char, char>, char> transitions; // {StateName, Input} -> NextStateName
        queue<set<int>> unvisited;
        
        char stateName = 'A';
        set<int> startState = root->firstpos;
        
        dfaStates[startState] = stateName++;
        unvisited.push(startState);

        cout << "\n--- DFA Transitions ---\n";
        
        while (!unvisited.empty()) {
            set<int> currentState = unvisited.front();
            unvisited.pop();
            char currentName = dfaStates[currentState];

            for (char symbol : alphabet) {
                set<int> nextState;
                // Union of followpos of all positions in currentState that match 'symbol'
                for (int pos : currentState) {
                    if (posToChar[pos] == symbol) {
                        nextState = mergeSets(nextState, followpos[pos]);
                    }
                }

                if (nextState.empty()) continue;

                if (dfaStates.find(nextState) == dfaStates.end()) {
                    dfaStates[nextState] = stateName++;
                    unvisited.push(nextState);
                }

                cout << currentName << " --(" << symbol << ")--> " << dfaStates[nextState] << "\n";
            }
        }

        cout << "\n--- Accepting States ---\n";
        int acceptPos = posCounter - 1; // The '#' symbol is always the last position
        for (const auto& state : dfaStates) {
            if (state.first.count(acceptPos)) {
                cout << state.second << " is an accepting state.\n";
            }
        }
    }

    void printFollowPos() {
        cout << "\n--- Followpos Table ---\n";
        for (const auto& pair : followpos) {
            cout << "Pos " << pair.first << " (" << posToChar[pair.first] << "): { ";
            for (int p : pair.second) cout << p << " ";
            cout << "}\n";
        }
    }
};

int main() {
    // Example: (a|b)*abb# translated to postfix with explicit concat (.)
    string postfix = "ab|*a.b.b.#."; 
    
    DirectDFA dfaBuilder;
    
    cout << "Parsing Postfix RE: " << postfix << "\n";
    Node* root = dfaBuilder.buildSyntaxTree(postfix);
    
    dfaBuilder.computeFunctions(root);
    dfaBuilder.printFollowPos();
    dfaBuilder.constructDFA(root);

    return 0;
}
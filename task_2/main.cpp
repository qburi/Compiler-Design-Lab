#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <cctype>

using namespace std;

bool isKeyword(const string& str) {
    vector<string> keywords = {
        "auto", "break", "case", "char", "const", "continue", "default", "do",
        "double", "else", "enum", "extern", "float", "for", "goto", "if",
        "int", "long", "register", "return", "short", "signed", "sizeof", "static",
        "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while",
        "main", "printf", "scanf"
    };
    for (const string& s : keywords) {
        if (s == str)
            return true;
    }
    return false;
}

bool isSpecialSymbol(char ch) {
    string syms = ",;(){}[]$";
    for (char c : syms) {
        if (c == ch)
            return true;
    }
    return false;
}

bool isOperatorChar(char ch) {
    string ops = "+-*/%<>=!";
    for (char c : ops) {
        if (c == ch)
            return true;
    }
    return false;
}

int main() {
    ifstream file("input.txt");
    if (!file.is_open()) {
        cout << "Unable to open input.txt" << endl;
        cout << "The file may not exist" << endl;
        return 1;
    }

    set<string> variables;
    set<string> literals;
    set<string> operators;
    set<string> constants;
    set<char> special_symbols;
    vector<string> non_tokens;

    string line;
    while (getline(file, line)) {
        int n = line.length();
        for (int i = 0; i < n; i++) {
            if (isspace(line[i])) continue;

            // for preprocessor directives -> capture the entire line
            if (line[i] == '#') {
                string directive = line.substr(i);
                non_tokens.push_back("Preprocessor Directive/Macro: " + directive);
                break; // Directives captures the entire line
            }

            // for single-line comments
            if (i + 1 < n && line[i] == '/' && line[i+1] == '/') {
                string comment = line.substr(i + 2);

                // left-strip whitespaces
                int start = 0;
                while(start < comment.length() && isspace(comment[start]))
                    start++;

                non_tokens.push_back("Comments: " + comment.substr(start));
                break;
            }

            // Handle Multi-line Comments
            if (i + 1 < n && line[i] == '/' && line[i+1] == '*') {
                int j = i + 2;
                string comment = "";
                bool closed = false;
                while (j < n - 1) {
                    if (line[j] == '*' && line[j+1] == '/') {
                        closed = true;
                        i = j + 1;
                        break;
                    }
                    comment += line[j++];
                }
                if (closed) {
                    int start = 0;

                    // left-strip whitespaces
                    while(start < comment.length() && isspace(comment[start]))
                        start++;

                    non_tokens.push_back("Comments: " + comment.substr(start));
                    continue;
                }
            }

            // for handling literals -> strings
            if (line[i] == '"') {
                string literal = "\"";
                int j = i + 1;
                while (j < n && line[j] != '"') {
                    literal += line[j++];
                }
                if (j < n) literal += '"';
                literals.insert(literal);
                i = j;
                continue;
            }

            // for variable and array names
            if (isalpha(line[i]) || line[i] == '_') {
                string id = "";
                while (i < n && (isalnum(line[i]) || line[i] == '_')) {
                    id += line[i++];
                }

                if (i < n && line[i] == '[') {
                    string arrayDef = id + "[";
                    i++;
                    while (i < n && line[i] != ']') {
                        arrayDef += line[i++];
                    }
                    if (i < n) arrayDef += "]";
                    if (!isKeyword(id)) {
                         variables.insert(arrayDef);
                    }
                } else {
                    i--;
                    if (!isKeyword(id)) {
                        variables.insert(id);
                    }
                }
                continue;
            }

            // for handling constants including negative numbers
            bool isNegativeNumber = false;
            if (line[i] == '-' && i + 1 < n && isdigit(line[i+1])) {
                int prev = i - 1;
                while (prev >= 0 && isspace(line[prev])) prev--;
                if (prev < 0 || isOperatorChar(line[prev]) || line[prev] == '(' || line[prev] == '=' || line[prev] == ',') {
                    isNegativeNumber = true;
                }
            }

            if (isdigit(line[i]) || isNegativeNumber) {
                string num = "";
                if (isNegativeNumber) {
                    num += '-';
                    i++;
                }
                while (i < n && isdigit(line[i])) {
                    num += line[i++];
                }
                constants.insert(num);
                i--;
                continue;
            }

            // for handling operators
            if (isOperatorChar(line[i])) {
                if (i + 1 < n) {
                    string op2 = line.substr(i, 2);
                    if (op2 == "==" || op2 == "!=" || op2 == ">=" || op2 == "<=" || op2 == "=<") {
                        operators.insert(op2);
                        i++;
                        continue;
                    }
                }
                operators.insert(string(1, line[i]));
                continue;
            }

            // Handle Special Symbols
            if (isSpecialSymbol(line[i])) {
                special_symbols.insert(line[i]);
                continue;
            }
        }
    }
    file.close();

    cout << "Tokens" << endl;

    cout << "variables: ";
    for (const string& v : variables) cout << v << " ";
    cout << endl;

    cout << "literals: ";
    for (const string& l : literals) cout << l << " ";
    cout << endl;

    cout << "operator: ";
    for (const string& o : operators) cout << o << " ";
    cout << endl;

    cout << "constants: ";
    for (const string& c : constants) cout << c << " ";
    cout << endl;

    cout << "special symbols or delimiters: ";
    for (char s : special_symbols) {
        if (s != '[' && s != ']') cout << s << " ";
    }
    cout << endl;

    if (!non_tokens.empty()) {
        cout << "\nNon tokens" << endl;
        for (const string& nt : non_tokens) {
            cout << nt << endl;
        }
    }

    return 0;
}
#include <bits/stdc++.h>
using namespace std;

string input;
int cursor = 0;
bool error_flag = false;

// Function prototypes for all non-terminals
void E();
void E_prime();
void T();
void T_prime();
void F();

// Utility function to match terminals
void match(char expected) {
    if (cursor < input.length() && input[cursor] == expected) {
        cursor++;
    } else {
        error_flag = true;
    }
}

// E -> T E'
void E() {
    T();
    E_prime();
}

// E' -> + T E' | epsilon
void E_prime() {
    if (cursor < input.length() && input[cursor] == '+') {
        match('+');
        T();
        E_prime();
    }
    // Epsilon transition: do nothing if '+' is not found
}

// T -> F T'
void T() {
    F();
    T_prime();
}

// T' -> * F T' | epsilon
void T_prime() {
    if (cursor < input.length() && input[cursor] == '*') {
        match('*');
        F();
        T_prime();
    }
    // Epsilon transition: do nothing if '*' is not found
}

// F -> ( E ) | id
void F() {
    if (cursor < input.length() && input[cursor] == '(') {
        match('(');
        E();
        match(')');
    }
    // Assuming 'id' is a single lowercase letter for simplicity
    else if (cursor < input.length() && islower(input[cursor])) {
        match(input[cursor]);
    }
    else {
        error_flag = true;
    }
}

int main() {
    cout << "Enter the string to parse (e.g., a+b*c): ";
    cin >> input;

    cursor = 0;
    error_flag = false;

    // Start parsing from the start symbol
    E();

    // If we parsed the whole string and hit no errors, it's valid
    if (cursor == input.length() && !error_flag) {
        cout << "String successfully parsed!" << endl;
    } else {
        cout << "Syntax Error." << endl;
    }

    return 0;
}
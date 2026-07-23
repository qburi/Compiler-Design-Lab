#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <iomanip>
#include <fstream>

using namespace std;

struct Symbol {
    string name;
    string type;
    string size;
    string address;
};

// function to get the byte size of standard C data types
string getSize(const string& type) {
    if (type == "int") return "4";
    if (type == "float") return "4";
    if (type == "double") return "8";
    if (type == "char") return "1";
    if (type == "void") return "-";
    return "-";
}

int main() {
    vector<Symbol> symbolTable;
    string line;

    // starting dummy address
    int current_address = 1001;

    // Regular Expressions for parsing
    // Matches functions like: int add(int a, int b)
    regex func_regex(R"(^\s*(int|float|char|double|void)\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\((.*?)\)\s*\{?)");

    // Matches simple variable declarations like: "int a=20;", "float sum;"
    regex var_regex(R"(^\s*(int|float|char|double)\s+([a-zA-Z_][a-zA-Z0-9_]*)(?:\s*=\s*[^;,]+)?\s*(?:,|;))");

    ifstream inputFile("input.txt");

    if (!inputFile.is_open()) {
        cerr << "Error: Could not open input.txt" << "\n";
        return 1;
    }

    while (getline(inputFile, line)) {
        smatch match;

        // 1. Check for Function Declarations and their parameters
        if (regex_search(line, match, func_regex)) {
            string params_str = match[3];

            // Parse function parameters if any exist
            if (!params_str.empty()) {
                regex param_regex(R"((int|float|char|double)\s+([a-zA-Z_][a-zA-Z0-9_]*))");
                sregex_iterator param_it(params_str.begin(), params_str.end(), param_regex);
                sregex_iterator param_end;

                while (param_it != param_end) {
                    smatch param_match = *param_it;
                    string p_type = param_match[1];
                    string p_name = param_match[2];

                    symbolTable.push_back({
                        p_name,
                        p_type,
                        getSize(p_type),
                        to_string(current_address)
                    });

                    current_address += stoi(getSize(p_type));
                    param_it++;
                }
            }
            continue; // Skip the standalone variable check for this specific line
        }

        // 2. Check for Variable Declarations
        if (regex_search(line, match, var_regex)) {
            string var_type = match[1];
            string var_name = match[2];

            symbolTable.push_back({
                var_name,
                var_type,
                getSize(var_type),
                to_string(current_address)
            });

            current_address += stoi(getSize(var_type));
        }
    }

    // Close the file when done reading
    inputFile.close();

    // Print Symbol Table
    cout << left << setw(15) << "NAME" 
         << setw(15) << "TYPE" 
         << setw(10) << "SIZE" 
         << setw(12) << "ADDRESS" << "\n";
    cout << string(52, '-') << "\n";

    for (const auto& sym : symbolTable) {
        cout << left << setw(15) << sym.name 
             << setw(15) << sym.type 
             << setw(10) << sym.size 
             << setw(12) << sym.address << "\n";
    }

    return 0;
}
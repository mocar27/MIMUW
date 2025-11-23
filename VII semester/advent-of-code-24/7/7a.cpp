#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

bool evaluate(const vector<long long>& v, long long sum, long long currentSum, int index) {
    if (currentSum == sum && index == v.size()) { return true; }
    else if (index == v.size()) { return false; }

    if (currentSum == 0) { 
        if (evaluate(v, sum, v[index], index + 1)) { return true; } 
    }
    else { 
        if (evaluate(v, sum, currentSum * v[index], index + 1)) { return true; } 
    }

    if (evaluate(v, sum, currentSum + v[index], index + 1)) { return true; }

    return false;
}

vector<string> split(const string &str, char delimiter) {
    vector<string> tokens;
    stringstream ss(str);
    string token;

    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

int main() {
    vector<string> lines;
    string input;
    long long result = 0;

    while (getline(cin, input)) {
        auto parts = split(input, ':');
        long long sum = stoll(parts[0]);

        auto values = split(parts[1], ' ');
        vector<long long> v;
        for (const auto& value : values) { 
            if (value != "") { v.emplace_back(stol(value)); }
        }
    
        if(evaluate(v, sum, 0, 0)) { result += sum; }
    }
    cout << result << endl;
}
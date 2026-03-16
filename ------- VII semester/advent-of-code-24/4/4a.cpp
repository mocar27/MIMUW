#include <iostream>
#include <vector>
#include <sstream>
#include <string>

using namespace std;

int check(const vector<vector<char> > &v, int i, int j) {
    int result = 0;
    int n = v.size();
    int m = v[0].size();
    // i + 
    // i - 
    // j + 
    // j - 
    // i + j + 
    // i + j - 
    // i - j + 
    // i - j - 

    if (i + 3 < n) {
        if (v[i+1][j] == 'M' && v[i+2][j] == 'A' && v[i+3][j] == 'S') { result++; }
        if (j + 3 < m) {
            if (v[i+1][j+1] == 'M' && v[i+2][j+2] == 'A' && v[i+3][j+3] == 'S') { result++; }
        }
        if (j - 3 >= 0) {
            if (v[i+1][j-1] == 'M' && v[i+2][j-2] == 'A' && v[i+3][j-3] == 'S') { result++; }
        }
    }

    if (i - 3 >= 0) {
        if (v[i-1][j] == 'M' && v[i-2][j] == 'A' && v[i-3][j] == 'S') { result++; }
        if (j + 3 < m) {
            if (v[i-1][j+1] == 'M' && v[i-2][j+2] == 'A' && v[i-3][j+3] == 'S') { result++; }
        }
        if (j - 3 >= 0) {
            if (v[i-1][j-1] == 'M' && v[i-2][j-2] == 'A' && v[i-3][j-3] == 'S') { result++; }
        }
    }

    if (j + 3 < m) {
        if (v[i][j+1] == 'M' && v[i][j+2] == 'A' && v[i][j+3] == 'S') { result++; }
    }

    if (j - 3 >= 0) {
        if (v[i][j-1] == 'M' && v[i][j-2] == 'A' && v[i][j-3] == 'S') { result++; }
    }

    return result;
}

int main() {
    string line;
    vector<vector<char> > v;

    while(getline(cin, line)) {
        int n = line.length();

        vector<char> temp;
        for (int i =0; i < n; i++) { temp.emplace_back(line[i]); }
        v.emplace_back(temp);
    }

    int result = 0;
    for (int i =0; i < v.size(); i++) {
        for (int j = 0; j < v[i].size(); j++) {
            if (v[i][j] == 'X') {
                result += check(v, i, j);
            }
        }
    }   

    cout << result << endl;
    return 0;
}
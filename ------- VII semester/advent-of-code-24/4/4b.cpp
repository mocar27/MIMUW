#include <iostream>
#include <vector>
#include <sstream>
#include <string>

using namespace std;

int check(const vector<vector<char> > &v, int i, int j) {
    int result = 0;
    int n = v.size();
    int m = v[0].size();

    // i + j + == i + j - == {S, M} || i + j + == i - j + == {S, M} \ {S v M} 

    if (i - 1 >= 0 && i + 1 < n && j - 1 >= 0 && j + 1 < m) {   // we won't leave the board
        if (v[i+1][j+1] == v[i-1][j+1] ) {
            if (v[i+1][j-1] == v[i-1][j-1]) {   // then top ones must be equal
                if (v[i+1][j+1] == 'S' && v[i+1][j-1] == 'M') { result++; }
                else if (v[i+1][j+1] == 'M' && v[i+1][j-1] == 'S') { result++; }
            }
        }
        else if (v[i+1][j+1] == v[i+1][j-1]) {
            if (v[i-1][j+1] == v[i-1][j-1]) {   // then left ones must be equal
                if (v[i+1][j+1] == 'S' && v[i-1][j+1] == 'M') { result++; }
                else if (v[i+1][j+1] == 'M' && v[i-1][j+1] == 'S') { result++; }
            }
        }
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
            if (v[i][j] == 'A') {
                result += check(v, i, j);
            }
        }
    }   

    cout << result << endl;
    return 0;
}
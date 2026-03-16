#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<vector<char> > v;

    string line;
    while(getline(cin, line)) {
        int n = line.length();

        vector<char> temp;
        for (int i =0; i < n; i++) { temp.emplace_back(line[i]); }
        v.emplace_back(temp);
    }

    int n = v.size();
    int i = -1;
    int j = -1;

    for (int k = 0; k < n; k++) {
        for (int l =0; l < n; l++) {
            if (v[k][l] == '^') {
                i = k;
                j = l;
                break;
            }
        }
    }

    int result = 1;
    bool inBounds = true;

    while(inBounds) {
        if (inBounds) {
            while (v[i-1][j] != '#') {
                i--;
                if (v[i][j] != 'X') {
                    result++;
                }
                v[i][j] = 'X';
                if (i - 1 < 0) {
                    inBounds = false;
                    break;
                }
            }
        }
        if (inBounds) {
            while (v[i][j+1] != '#') {
                j++;
                if (v[i][j] != 'X') {
                    result++;
                }
                v[i][j] = 'X';
                if (j + 1 >= n) {
                    inBounds = false;
                    break;
                }
            }
        }
        if (inBounds) {
            while (v[i+1][j] != '#') {
                i++;
                if (v[i][j] != 'X') {
                    result++;
                }
                v[i][j] = 'X';
                if (i + 1 >= n) {
                    inBounds = false;
                    break;
                }
            }
        }
        if (inBounds) {
            while (v[i][j-1] != '#') {
                j--;
                if (v[i][j] != 'X') {
                    result++;
                }
                v[i][j] = 'X';
                if (j - 1 < 0) {
                    inBounds = false;
                    break;
                }
            }
        }
    }
    cout << result << endl;
}
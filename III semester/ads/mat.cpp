#include <bits/stdc++.h>
using namespace std;

string s;

int main() {

    cin >> s;
    int minDistance = s.size() - 1;
    int tempDistance = 0;
    char lastChar = '*';
    for (int i = 0; i < s.size(); i++) {
        if (s[i] != '*') {

            if (s[i] != lastChar && lastChar != '*' && tempDistance < minDistance) {
                minDistance = tempDistance;
            }

            tempDistance = 0;
            lastChar = s[i];
        }
        else {
            tempDistance++;
        }
    }

    cout << s.size() - minDistance;
}
#include <bits/stdc++.h>
using namespace std;

#define modulo 1000000000;    

int n;
int s[1005];
long long L[1005][1005], R[1005][1005];

void fillL(long long L[][1005], long long R[][1005], int i, int j);
void fillR(long long L[][1005], long long R[][1005], int i, int j);

void fillL(long long L[][1005], long long R[][1005], int i, int j) {

    if (i == j) {
        L[i][j] = 1;
    }

    else if (j - i == 1) {
        if (s[i] < s[i + 1]) {
            L[i][j] = 1;
        }
        else {
            L[i][j] = 0;
        }
    }

    if (j - i >= 2) {

        if (L[i + 1][j] == -1) {
            fillL(L, R, i + 1, j);
        }

        if (R[i + 1][j] == -1) {
            fillR(L, R, i + 1, j);
        }

        L[i][j] = ((s[i] < s[i + 1]) * L[i + 1][j] + (s[i] < s[j]) * R[i + 1][j]) % modulo;
    }
}

void fillR(long long L[][1005], long long R[][1005], int i, int j) {

    if (i == j) {
        R[i][j] = 1;
    }

    else if (j - i == 1) {
        if (s[i] < s[j]) {
            R[i][j] = 1;
        }
        else {
            R[i][j] = 0;
        }
    }

    if (j - i >= 2) {

        if (L[i][j - 1] == -1) {
            fillL(L, R, i, j - 1);
        }
        if (R[i][j - 1] == -1) {
            fillR(L, R, i, j - 1);
        }

        R[i][j] = ((s[i] < s[j]) * L[i][j - 1] + (s[j - 1] < s[j]) * R[i][j - 1]) % modulo;
    }
}

int main() {

    ios_base::sync_with_stdio(0);
    cin.tie(NULL);
    cout.tie(NULL);

    cin >> n;
    for (int i = 0; i < n; i++) {
        cin >> s[i];
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            L[i][j] = -1;
            R[i][j] = -1;
        }
    }

    fillL(L, R, 0, n - 1);
    fillR(L, R, 0, n - 1);

    // cout << endl;
    // for (int i = 0; i < n; i++) {
    //     for (int j = 0; j < n; j++) {
    //         cout << L[i][j] << " ";
    //     }
    //     cout << endl;
    // }
    // cout << endl;

    // for (int i = 0; i < n; i++) {
    //     for (int j = 0; j < n; j++) {
    //         cout << R[i][j] << " ";
    //     }
    //     cout << endl;
    // }
    // cout << endl;

    if (n == 1) {
        cout << L[0][n - 1] % modulo;
        return 0;
    }
    else {
        cout << (L[0][n - 1] + R[0][n - 1]) % modulo;
        return 0;
    }
}
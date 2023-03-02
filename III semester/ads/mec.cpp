#include <bits/stdc++.h>
using namespace std;

int n, m;

int player;
set<long long> plays;
long long allLabels[40005];

int main() {


    ios_base::sync_with_stdio(0);
    cin.tie(NULL);
    cout.tie(NULL);

    cin >> n >> m;

    if (n == 1) {
        cout << "TAK";
        return 0;
    }

    for (int i = 0; i < m; i++) {
        for (int j = 1; j <= n; j++) {
            cin >> player;
            if (j > n / 2) {
                allLabels[player] = allLabels[player] | (1LL << i);
            }
        }
    }

    bool result = true;
    sort(allLabels + 1, allLabels + n + 1);
    for (int i = 2; i <= n; i++) {
        if (allLabels[i] == allLabels[i - 1]) {
            result = false;
        }
    }
    // for (int i = 1; i <= n; i++) {
    //     if (!plays.count(allLabels[i])) {
    //         plays.insert(allLabels[i]);
    //     }
    //     else {
    //         result = false;
    //     }
    // }

    result ? cout << "TAK" : cout << "NIE";
}
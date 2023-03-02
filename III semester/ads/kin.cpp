#include <bits/stdc++.h>
using namespace std;

const long long modulo = 1000000000;
const long long b = 65536;  // 2^16

int n, k, s;
long long tab[11][b * 2];

void add(int currentK, int p, long long num) {

    p += b;
    num -= tab[currentK][p];

    while (p > 0) {
        tab[currentK][p] += num;
        tab[currentK][p] %= modulo;
        p /= 2;
    }
}

long long query(int currentK, int first, int last) {

    long long res = 0;
    first += b;
    last += b;

    if (first != last) {
        res = ((tab[currentK][first] + tab[currentK][last]) % modulo);
    }
    else {
        res = (tab[currentK][first]) % modulo;
    }

    while (first / 2 != last / 2 /* && first > 0 && last > 0 */) {

        if (first % 2 == 0) {
            res = (res + tab[currentK][first + 1]) % modulo;
        }

        if (last % 2 == 1) {
            res = (res + tab[currentK][last - 1]) % modulo;
        }

        first /= 2;
        last /= 2;
    }

    return res % modulo;
}

int main() {

    ios_base::sync_with_stdio(0);
    cin.tie(NULL);
    cout.tie(NULL);

    cin >> n >> k;
    // add(0, n, 1);

    for (int i = 0; i < n; i++) {
        cin >> s;
        add(1, s, 1);

        for (int j = k; j >= 2; j--) {
            add(j, s + 1, query(j - 1, s + 1, n));
        }
    }

    cout << query(k, 0, n);
}

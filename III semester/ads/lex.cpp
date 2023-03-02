#include <bits/stdc++.h>
using namespace std;

int n, m;
string s;
int a, b, c, d;

const long long p = 997;
const long long modulo = 1e9 + 7;
long long hashtable[300005];
long long powers[300005];


int binSearch(int l1, int r1, int l2, int r2) {

    int l = 0, r = min(r1 - l1 + 1, r2 - l2 + 1);

    while (l != r) {
        int mid = (l + r) / 2;

        if ((hashtable[l1 + mid] - (hashtable[l1 - 1] * powers[mid + 1] + modulo) % modulo + modulo) % modulo ==
            (hashtable[l2 + mid] - (hashtable[l2 - 1] * powers[mid + 1] + modulo) % modulo + modulo) % modulo) {
            l = mid + 1;
        }
        else {
            r = mid;
        }
    }

    return l;
}

int query(int l1, int r1, int l2, int r2) {

    int length = binSearch(l1, r1, l2, r2);
    int left = r1 - l1 + 1;
    int right = r2 - l2 + 1;

    if (length == left && length == right) {
        return 0;
    }
    else if (length == left) {
        return -1;
    }
    else if (length == right) {
        return 1;
    }
    else if (s[l1 + length] < s[l2 + length]) {
        return -1;
    }
    else {
        return 1;
    }
}

int main() {

    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);

    cin >> n >> m;
    cin >> s;
    s = " " + s;

    for (long long i = 1; i <= n; i++) {
        hashtable[i] = ((hashtable[i - 1] * p + modulo) % modulo + s[i] + modulo) % modulo;
    }

    powers[0] = 1;
    for (long long i = 1; i <= n + 1; i++) {
        powers[i] = ((powers[i - 1] * p + modulo) % modulo + modulo) % modulo;
    }

    for (int i = 0; i < m; i++) {
        cin >> a >> b >> c >> d;

        switch (query(a, b, c, d)) {
        case -1:
            cout << "<\n";
            break;
        case 1:
            cout << ">\n";
            break;
        default:
            cout << "=\n";
            break;
        }
    }
}
#include <bits/stdc++.h>
using namespace std;

int n, m, k;
int a, b;
vector<vector<int>> adjLists;
long long  p[100007];
int d[100007];
priority_queue <pair<int, int>, vector < pair<int, int>>, greater<pair<int, int>>> helperQue;
vector<int> result;

int main() {

    ios_base::sync_with_stdio(0);
    cin.tie(NULL);
    cout.tie(NULL);

    cin >> n >> m >> k;
    adjLists.resize(n + 1);

    for (int i = 1; i <= n; i++) {
        cin >> p[i];
    }

    for (int i = 0; i < m; i++) {
        cin >> a >> b;
        adjLists[b].push_back(a);
        d[a]++;
    }

    for (int i = 1; i <= n; i++) {
        if (d[i] == 0) {
            helperQue.push(make_pair(p[i], i));
        }
    }

    for (int i = 0; i < k; i++) {
        result.push_back(helperQue.top().first);
        int v = helperQue.top().second;
        helperQue.pop();
        for (int w : adjLists[v]) {
            d[w]--;
            if (d[w] == 0) {
                helperQue.push(make_pair(p[w], w));
            }
        }
    }

    int r = 0;
    for (int i = 0; i < result.size(); i++) {
        r = max(r, result[i]);
    }

    cout << r;
}
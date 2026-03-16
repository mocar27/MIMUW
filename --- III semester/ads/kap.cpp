#include <bits/stdc++.h>
using namespace std;

const long long inf = 1e9 + 7;

int n;
vector<long long> d;
pair<long, long> x[200007], y[200007];
vector<pair<long, long>> adjList[200007];

struct customComparator {
    bool operator()(const int& a, const int& b) const {
        if (d[a] < d[b]) {
            return true;
        }
        if (d[a] > d[b]) {
            return false;
        }
        return a < b;
    }
};

long long dijkstra(int startingIsland) {

    d.resize(n + 1);
    fill(d.begin(), d.end(), inf);

    set<int, customComparator> mySet;
    mySet.insert(startingIsland);

    d[startingIsland] = 0;

    while (mySet.size() > 0) {
        int currentNode = *mySet.begin();
        mySet.erase(mySet.begin());

        for (auto it : adjList[currentNode]) {
            int nextNode = it.first;
            long long weight = it.second;

            if (d[nextNode] > d[currentNode] + weight) {
                mySet.erase(nextNode);
                d[nextNode] = d[currentNode] + weight;
                mySet.insert(nextNode);
            }
        }
    }

    return d[n];
}


int main() {

    ios_base::sync_with_stdio(0);
    cin.tie(NULL);
    cout.tie(NULL);

    cin >> n;

    for (int i = 1; i <= n; i++) {
        int myX, myY;

        cin >> myX >> myY;

        x[i] = make_pair(myX, i);
        y[i] = make_pair(myY, i);
    }

    sort(x + 1, x + n + 1);
    sort(y + 1, y + n + 1);

    for (int i = 1; i <= n; i++) {
        if (i != 1) {
            adjList[x[i].second].push_back(make_pair(x[i - 1].second, x[i].first - x[i - 1].first));
            adjList[y[i].second].push_back(make_pair(y[i - 1].second, y[i].first - y[i - 1].first));
        }
        if (i != n) {
            adjList[x[i].second].push_back(make_pair(x[i + 1].second, x[i + 1].first - x[i].first));
            adjList[y[i].second].push_back(make_pair(y[i + 1].second, y[i + 1].first - y[i].first));
        }
    }

    cout << dijkstra(1);
}
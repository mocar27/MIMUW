#include <bits/stdc++.h>
using namespace std;

const int log_n = 20;

struct node {
    vector<int> adj;
    int furthest;
    int distToFurthest;
};
typedef struct node node;

int n, m;
int fathers[500007][log_n + 1];
int depth[500007];
int preorder[500007];
int postorder[500007];
vector<node> adjLists;

int c;
int len;
int tab[2];

void dfs_set_fathers(int u, int parent) {

    preorder[u] = c++;
    fathers[u][0] = parent;
    depth[u] = depth[parent] + 1;

    for (int i = 1; i <= log_n - 1; i++) {
        fathers[u][i] = fathers[fathers[u][i - 1]][i - 1];
    }
    if (depth[u] > len) {
        len = depth[u] - 1;
        tab[0] = u;
    }

    for (int i = 0; i < adjLists[u].adj.size(); i++) {
        int w = adjLists[u].adj[i];
        if (w != parent) {
            dfs_set_fathers(w, u);
        }
    }
    postorder[u] = c++;
}

void dfs_distance0(int u, int parent, int d) {

    if (adjLists[u].distToFurthest < d) {
        adjLists[u].furthest = tab[0];
    }

    adjLists[u].distToFurthest = max(d, adjLists[u].distToFurthest);

    if (d >= len) {
        len = d;
        tab[1] = u;
    }

    for (int i = 0; i < adjLists[u].adj.size(); i++) {
        int w = adjLists[u].adj[i];
        if (w != parent) {
            dfs_distance0(w, u, d + 1);
        }
    }
}

void dfs_distance1(int u, int parent, int d) {

    if (adjLists[u].distToFurthest < d) {
        adjLists[u].furthest = tab[1];
    }

    adjLists[u].distToFurthest = max(d, adjLists[u].distToFurthest);

    for (int i = 0; i < adjLists[u].adj.size(); i++) {
        int w = adjLists[u].adj[i];
        if (w != parent) {
            dfs_distance1(w, u, d + 1);
        }
    }
}

int findNodeInTree(int u, int dist) {
    for (int i = 0; i < log_n; i++) {
        if (dist & (1 << i)) {
            u = fathers[u][i];
        }
    }
    return u;
}

int lca(int u, int w) {

    if (depth[u] < depth[w]) {
        return lca(w, u);
    }

    int d = depth[u] - depth[w];
    u = findNodeInTree(u, d);

    if (u == w) {
        return u;
    }

    int x = u;
    int y = w;

    for (int i = log_n - 1; i >= 0; i--) {
        if (fathers[x][i] != fathers[y][i]) {
            x = fathers[x][i];
            y = fathers[y][i];
        }
    }
    return fathers[x][0];
}



int findNode(int start, int distance) {
    int u = adjLists[start].furthest;
    int w = lca(start, u);

    if (distance <= depth[start] - depth[w]) {
        return findNodeInTree(start, distance);
    }

    return findNodeInTree(u, (depth[u] - depth[w]) - distance + (depth[start] - depth[w]));
}

int query(int start, int distance) {
    return distance > adjLists[start].distToFurthest ? -1 : findNode(start, distance);
}

int main() {

    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);

    cin >> n;
    adjLists.resize(n + 2);

    for (int i = 1; i <= n; i++) {
        int a, b;
        cin >> a >> b;
        if (a != -1) {
            adjLists[i].adj.push_back(a);
            adjLists[a].adj.push_back(i);
        }
        if (b != -1) {
            adjLists[i].adj.push_back(b);
            adjLists[b].adj.push_back(i);
        }
        adjLists[i].distToFurthest = -1;
    }

    dfs_set_fathers(1, 1); // dfs_pre_post(1, 0)?
    dfs_distance0(tab[0], tab[0], 0);
    dfs_distance1(tab[1], tab[1], 0);

    cin >> m;

    for (int i = 0; i < m; i++) {
        int v, d;
        cin >> v >> d;
        cout << query(v, d) << endl;
    }
}
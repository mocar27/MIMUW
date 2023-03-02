#include <bits/stdc++.h>
using namespace std;

struct node {
    /* bedziemy w kazdym wezle drzewa przedzialowego trzymac dwa kolory
       ktore sa dominujace w danym poddrzewie,
       w lisciach tylko jedna para bedzie niezerowa,
       bedzie to kolor ktory jest aktualnie w naszym wezle,
       obieg preorder i tablica maxPreorder zapewni nam ze bedziemy
       dobrze przebiegac i sprawdzac konkretne wierzcholki dla drzewa z zadania,
       trzymamy dwie pary kolorow dominujacych, bo dla przedzialu dlugosci 2,
       nie wiemy jaki kolor jest dominujacy, wiec chcemy trzymac oba,
       a pozniej wybierac ten ktory rzeczywiscie jest dominujacy */
    pair<int, int> leftPair = { 0, 0 };
    pair<int, int> rightPair = { 0, 0 };

    void updateLeft(int f, int s) {
        leftPair.first = f;
        leftPair.second = s;
    }

    void updateRight(int f, int s) {
        rightPair.first = f;
        rightPair.second = s;
    }

    pair<int, int> getLeft() {
        return leftPair;
    }

    pair<int, int> getRight() {
        return rightPair;
    }
};

struct Tree {

    /* wektory potrzebne do puszczenia dfs
       oraz zrobienia obiegu preorder na drzewie
       podanym w zadaniu */
    vector<vector<int>> adjList;
    vector<int> preorder;
    vector<int> maxPreorder; // maksymalny preorder w poddrzewie 

    // rzeczywiste drzewo przedzialowe
    int base = 1;
    vector<struct node> myTree;

    Tree(int n):
        adjList(n),
        preorder(n),
        maxPreorder(n) {
    }

    void setBase(int b) {
        while (base < b) {
            base *= 2;
        }
        myTree.resize(base * 2);
    }

    void insertAdjacent(int first, int last) {
        adjList[first].push_back(last);
    }

    int counter = -1;
    void DFS(int child, int parent) {
        preorder[child] = ++counter;
        for (int adj : adjList[child]) {
            if (adj != parent) {
                DFS(adj, child);
            }
        }
        maxPreorder[child] = counter;
    }

    // wybiera dwa kolory dominujace w poddrzewie drzewa przedzialowego
    struct node checkDominant(struct node left, struct node right) {

        array<pair<int, int>, 4> resultPairs = {
            left.getLeft(),
            left.getRight(),
            right.getLeft(),
            right.getRight()
        };

        /* sprawdzamy kolejno wszystkie pary wybierajac kolor dominujacy,
           a gdy jakies dwie pary okaza sie takie same to dodajem ilosc
           jednej pary do drugiej i resetujemy druga pare z tablicy,
           poniewaz nie bedzie nam juz aktualnie potrzebna */
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < i; j++) {
                if (resultPairs[i].second == resultPairs[j].second) {
                    resultPairs[i].first += resultPairs[j].first;
                    resultPairs[j] = { 0,0 };
                }
            }
        }

        /* wybieramy dwa kolory dominujace -
           te pierwsze dwa ktore w poddrzewie wystapily najczesciej */
        struct node result;
        sort(resultPairs.rbegin(), resultPairs.rend());
        result.updateLeft(resultPairs[0].first, resultPairs[0].second);
        result.updateRight(resultPairs[1].first, resultPairs[1].second);
        return result;
    }

    void change(int pos, int color) {
        pos += base;
        myTree[pos].updateLeft(1, color);
        myTree[pos].updateRight(0, -1);
        pos /= 2;
        /* zawsze poprawne bo sa conajmniej 2 liscie, bo n >= 2
           wiec zawsze podamy od dwoch lisci w gore dobrze.
           Po zmianie koloru znowu sprawdzamy jakie kolory dominuja
           w poddrzewie drzewa przedzialowego */
        while (pos >= 1) {
            myTree[pos] = checkDominant(myTree[pos * 2], myTree[pos * 2 + 1]);
            pos /= 2;
        }
    }

    /* wychodzimy od liscia w gore drzewa przedzialowego
     i sprawdzamy kolejno przedzialy
     w zaleznosci od naszej kolejnosci w tablicy preorder
     oraz od elementu w tablicy maksymalny preorder
     dla danego wezla */
    bool query(int first, int maxP) {
        first += base;
        maxP += base;
        struct node result;
        int len = maxP - first + 1;

        if (first == maxP) {
            result = myTree[first];
        }
        else {
            result = checkDominant(myTree[first], myTree[maxP]);
            while (first + 1 < maxP) {
                if (first % 2 == 0) {
                    result = checkDominant(result, myTree[first + 1]);
                }
                if (maxP % 2 != 0) {
                    result = checkDominant(result, myTree[maxP - 1]);
                }
                first /= 2;
                maxP /= 2;
            }
        }
        return result.getLeft().first >= len - 1;
    }
};

int main() {

    ios_base::sync_with_stdio(0);
    cin.tie(NULL);
    cout.tie(NULL);

    int n, q;
    cin >> n >> q;

    Tree christmasTree(n);
    for (int i = 1; i <= n - 1; i++) {
        int a;
        cin >> a;
        christmasTree.insertAdjacent(a - 1, i);
    }

    christmasTree.DFS(0, 0);
    christmasTree.setBase(n);

    for (int i = 0; i < n; i++) {
        int k;
        cin >> k;
        christmasTree.change(christmasTree.preorder[i], k);
    }

    char operation;
    for (int i = 0; i < q; i++) {
        cin >> operation;
        int v;
        switch (operation) {
        case '?':
            cin >> v;
            christmasTree.query(christmasTree.preorder[v - 1], christmasTree.maxPreorder[v - 1]) ?
                cout << "TAK\n" : cout << "NIE\n";
            break;

        case 'z':
            int x;
            cin >> v >> x;
            christmasTree.change(christmasTree.preorder[v - 1], x);
            break;
        }
    }
}
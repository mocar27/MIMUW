#include <iostream>
#include <fstream>
#include <queue>
#include <cmath>
#include <vector>

using namespace std;

int main () {
    priority_queue<int, vector<int>, greater<int> > p;
    priority_queue<int, vector<int>, greater<int> > q;

    ifstream fin;
    fin.open("1.input");

    int a;
    int b;
    while (!fin.eof()) {
        fin >> a;
        fin >> b;
        p.push(a);
        q.push(b);
    }
    fin.close();

    long long result = 0;
    while(!p.empty()) {
        long long dist = abs(p.top() - q.top());
        result += dist;
        p.pop();
        q.pop();
    }

    cout << result << endl;
    return 0;
}
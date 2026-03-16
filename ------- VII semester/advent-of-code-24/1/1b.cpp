#include <iostream>
#include <fstream>
#include <map>
#include <cmath>
#include <set>

using namespace std;

int main () {
    set<int> s;
    map<int, int> m;

    ifstream fin;
    fin.open("1.input");

    int a;
    int b;
    while (!fin.eof()) {
        fin >> a;
        fin >> b;
        s.insert(a);
        m[b]++;
    }
    fin.close();

    long long result = 0;
    for (int i : s) {
        result += i * m[i];
    }

    cout << result << endl;
    return 0;
}
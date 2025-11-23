#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <sstream>

using namespace std;

bool isSafe(vector<int>& v) { 
    int n = v.size();
    bool increasing = false;
    if (v[0] < v[1]) { increasing = true; }
    else if (v[0] > v[1]) { increasing = false; }
    else { return false; }

    for (int i =0; i < n - 1; i++) {
        int diff = abs(v[i] - v[i+1]);
        if (diff < 1 || diff > 3) { return false; }
        if (increasing && v[i] > v[i+1]) { return false; }
        if (!increasing && v[i] < v[i+1]) { return false; }
    }

    return true;
}

int main () {
    string line; 
    int result = 0;

    while (getline(cin, line)) {
        istringstream ss(line);
        int x;
        vector<int> v;

        while (ss >> x) { v.emplace_back(x); }
        int n = v.size();

        for (int i = 0; i < n; i++) {
            vector<int> u;
            for (int j = 0; j < n; j++) {
                if (j != i ) { u.emplace_back(v[j]); }
            }
            if (isSafe(u)) { result++; break; }
        }

    }
    cout << result << endl;
    return 0;
}

#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <sstream>

using namespace std;

int main () {
    string line; 
    int result = 0;
    set<int> d;
    d.insert(1);
    d.insert(2);
    d.insert(3);

    while (getline(cin, line)) {
        istringstream ss(line);
        int n;
        vector<int> v;

        while (ss >> n) { v.push_back(n); }

        int s = v.size();
        bool increasing = false;
        if (v[0] < v[1]) { increasing = true; }
        else if (v[0] > v[1]) { increasing = false; }

        bool isOk = true;
        for (int i = 0; i < s - 1;  i++) {
            int diff = abs(v[i] - v[i + 1]);

            if (increasing && v[i] < v[i + 1]) {
                if (!d.count(diff)) {
                    isOk = false;
                    break;
                }
            }
            else if (!increasing && v[i] > v[i + 1]) {
                if (!d.count(diff)) {
                    isOk = false;
                    break;
                }
            }
            else { 
                isOk = false; 
                break; 
            }
        }
        if (isOk) { result++; }
    }
    
    cout << result << endl;
    return 0;
}
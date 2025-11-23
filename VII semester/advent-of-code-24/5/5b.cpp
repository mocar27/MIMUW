#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>

using namespace std;

int main() {
    map<int, set<int> > good;
    map<int, set<int> > wrong;
    // on encounter of each number, 
    // check whether in the right side of that number there is a number that is wrong eg.
    // if there is rule 47|53, on encounter of 53, check numbers on the right side of 53
    // whether there occured 47 (and any other number that should not occure)
    
    ifstream fin;
    fin.open("5-1.input");

    int a, b;
    char c;
    while (!fin.eof()) {
        fin >> a;
        fin >> c;
        fin >> b;
        
        wrong[b].insert(a);
    }
    fin.close();

    fin.open("5-2.input");
    string line;
    int result = 0;
    while (getline(fin, line)) {
        istringstream ss(line);
        int x;
        char c;
        vector<int> v;

        while (ss >> x) {
            v.emplace_back(x);
            ss >> c;
        }


        // O(n^2)
        int n = v.size();
        bool isOk = true;
        for (int i =0; i < n; i++) {
            for (int j = i+1; j < n; j++) {
                if (wrong[v[i]].count(v[j]) != 0) { isOk = false; }
            }
        }
        if (!isOk) {
            // here the update is not ok, we want to fix it
            for (int i =0; i<n; i++) {
                for (int j = i+1; j < n; j++) {
                    if (wrong[v[i]].count(v[j]) != 0) { 
                        int t = v[i];
                        v[i] = v[j];
                        v[j] = t;
                    }
            }
            }

            n % 2 == 0 ?  result += v[(n-1)/2] : result += v[n/2];
        }
        
    }

    fin.close();
    cout << result << endl;
}

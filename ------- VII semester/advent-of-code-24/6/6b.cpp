#include <iostream>
#include <vector>
#include <map>
using namespace std;

// jeśli odwiedzi jakąś pozycje dwukrotnie i patrzy w tę samą stronęsto znaczy, że jest w pętli

enum Direction { UP, RIGHT, DOWN, LEFT };

int main() {
    vector<vector<char> > v;
    map<pair<int, int>, bool> visited;
    map<pair<int, int>, Direction> direction;

    string line;
    while(getline(cin, line)) {
        int n = line.length();

        vector<char> temp;
        for (int i =0; i < n; i++) { temp.emplace_back(line[i]); }
        v.emplace_back(temp);
    }

    int n = v.size();
    int i = -1;
    int j = -1;

    for (int k = 0; k < n; k++) {
        for (int l =0; l < n; l++) {
            if (v[k][l] == '^') {
                i = k;
                j = l;
                break;
            }
        }
    }

    int result = 0;
    bool inBounds = true;

    for (int k = 0; k < n; k++) {
        for (int l = 0; l < n; l++) {
            // setup of testing the obstacle placement
            int s = i;
            int t = j;
            // cout << "testing " << k << " " << l << endl;
            inBounds = true;
            visited.clear();
            direction.clear();

            while (v[k][l] == '#') l++;
            if (l >= n) break;

            v[k][l] = '#';

            while (inBounds) {
                if (inBounds) {
                    while (v[s-1][t] != '#') {
                        s--;
                        pair<int, int> p = make_pair(s, t);
                        if (visited[p] && direction[p] == UP) {
                            result++;
                            inBounds = false;
                            break;
                        }

                        visited[p] = true;
                        direction[p] = UP;

                        if (s - 1 < 0) {
                            inBounds = false;
                            break;
                        }
                    }
                }
                if (inBounds) {
                    while (v[s][t+1] != '#') {
                        t++;
                        pair<int, int> p = make_pair(s, t);
                        if (visited[p] && direction[p] == RIGHT) {
                            result++;
                            inBounds = false;
                            break;
                        }

                        visited[p] = true;
                        direction[p] = RIGHT;

                        if (t + 1 >= n) {
                            inBounds = false;
                            break;
                        }
                    }
                }
                if (inBounds) {
                    while (v[s+1][t] != '#') {
                        s++;
                        pair<int, int> p = make_pair(s, t);
                        if (visited[p] && direction[p] == DOWN) {
                            result++;
                            inBounds = false;
                            break;
                        }

                        visited[p] = true;
                        direction[p] = DOWN;

                        if (s + 1 >= n) {
                            inBounds = false;
                            break;
                        }
                    }
                }
                if (inBounds) {
                    while (v[s][t-1] != '#') {
                        t--;
                        pair<int, int> p = make_pair(s, t);
                        if (visited[p] && direction[p] == LEFT) {
                            result++;
                            inBounds = false;
                            break;
                        }

                        visited[p] = true;
                        direction[p] = LEFT;

                        if (t - 1 < 0) {
                            inBounds = false;
                            break;
                        }
                    }
                }
            }
            v[k][l] = '.';
        }
    }
    cout << result << endl;
}
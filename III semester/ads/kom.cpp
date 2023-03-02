#include <bits/stdc++.h>
using namespace std;

constexpr  long long modulo = 1000000007;
constexpr  long long inf = (int)1e9 + 11;

long long n, k, l;
long long workers[500005];
long long sums[500005];
long long sizeOfCommittee, numOfWays;

int main() {

    ios_base::sync_with_stdio(0);
    cin.tie(NULL);
    cout.tie(NULL);

    cin >> n >> k >> l;

    workers[0] = -inf;
    workers[n + 1] = inf;

    for (int i = 1; i <= n; i++) {
        cin >> workers[i];
    }

    sort(workers + 1, workers + n + 1);
    vector<pair< long long, long long>> dp(n + 1, { 0,0 });

    dp[0].first = -inf;
    dp[0].second = -inf;
    dp[1].first = 1;
    dp[1].second = 1;

    long long it = 2;

   // ustawiamy w pierwszych robotnikach [minimalny komitet](.first) w ktorych sa zawarci od [0...i] na 1 
   // bo oni sami moga pokryc samodzielnie caly taki komitet na lewo od siebie
   // i dla kazdego jednoosobowego komitetu mozna go wybrac tylko raz wiec ustawiamy
   // dla kazdego i [ile jest takich komitetow](.second) na 1
    for (it = 2; workers[it] - workers[1] <= k && it <= n; it++) {
        dp[it].first = 1;
        dp[it].second = 1;
    }

    // potem iterujemy sie tylko po robotnikach ktorzy sami nie moga pokryc 
    // calego komitetu na przedziale [0..n], tj. w wektorze maja pary (0,0)
    // for (int i = it; i <= n;i++) {

    //     // najdalszy pracownik ktorego juz nie moze reprezentowac workers[i]
    //     int minWorker = i - 1; // workers[i] nie moze juz reprezentowac workers[minWorker],
    //     int j = i - 1; // pierwszy z lewej ktory juz nie moze byc w komitecie z workers[i]

    //     // checkujemy wszystkich pracownikow ktorzy maja <= .first, 
    //     // szukamy najmniejszego .first bo minimalny komitet
    //     // jak znajdzie sie mniejszy niz aktualny to mamy komitet z mniajsza minimalna liczba osob

    //     while (minWorker > 1 && abs(workers[minWorker] - workers[i]) <= k) {// dopoki nie znajdzie pierwszego co nie wchodzi w przeciecie
    //         minWorker--;
    //     }
    //     while (j >= 1 && workers[i] - workers[j] < l) {  // workers[j] to pierwszy robotnik ktory moze byc w komitecie z workers[i]
    //         j--;
    //     }

    //     cout << "dla pracownika: " << workers[i] << " pierwszy z ktorym moze byc w komitecie to: " << workers[j] << " pierwszy nie reprezentowany: " << workers[minWorker] << endl;
    //     int minSize = inf;
    //     int sum = 0;



    //     if (dp[j].first != -inf) {
    //         minSize = dp[j].first;
    //     }

    //     for (int itr = j; itr >= 1; itr--) {

    //         if (abs(workers[minWorker] - workers[itr]) <= k) {   // jesli znajdziemy na lewo kogos z mniejsza liczba minimalnego komitetu
    //                     // to zamieniamy min na niego i sumujemy od niego od nowa 
    //             if (dp[itr].first != 0 && dp[itr].first < minSize) {
    //                 minSize = dp[itr].first;
    //                 sum = dp[itr].second % modulo;
    //             }
    //             else if (minSize != inf && dp[itr].first == minSize) {
    //                 sum = (sum + dp[itr].second) % modulo;
    //             }
    //         }
    //     }

    // // czy ten warunek jest dobry i wystarczajacy?
    //     if (minSize != inf && sum != 0) { // workers[minWorker] != -inf
    //         dp[i].first = minSize + 1;
    //         dp[i].second = sum;
    //     } // else workers[minWorker] == -inf wtedy zapisz minSize i sum
    // }

    long long w = 1;
    long long tail = 1;
    long long j = 1;
    long long minSize = 1;
    set < long long> counter;

    for (int i = it; i <= n; i++) {

        while (j < i && workers[j] + k < workers[i]) {
            j++;
        }
        j > 1 ? j-- : j;

        while (w < i && workers[i] - workers[w] >= l) {
            sums[dp[w].first] += dp[w].second;
            sums[dp[w].first] %= modulo;
            if (dp[w].first != 0) {
                counter.insert(dp[w].first);
            }
            minSize = min(minSize, dp[w].first);
            w++;
        }

        while (tail < w && workers[tail] + k < workers[j]) {
            sums[dp[tail].first] = (sums[dp[tail].first] - dp[tail].second + modulo) % modulo;
            if (sums[dp[tail].first] == 0) {
                counter.erase(dp[tail].first);
            }
            tail++;
        }

        if (!counter.empty() && sums[minSize] == 0) {
            minSize = *counter.begin();
        }

        if (sums[minSize] != 0) {
            dp[i].first = minSize + 1;
            dp[i].second = sums[minSize] % modulo;
        }
    }

    sizeOfCommittee = inf;
    for (int i = n; i >= 1 && workers[n] - workers[i] <= k; i--) {
        if (dp[i].first != 0 && dp[i].first < sizeOfCommittee) {
            sizeOfCommittee = dp[i].first;
            numOfWays = dp[i].second % modulo;
        }
        else if (sizeOfCommittee != inf && dp[i].first == sizeOfCommittee) {
            numOfWays = (numOfWays + dp[i].second) % modulo;
        }
    }
    // for (int i = 0; i <= n;i++) {
    //     cout << dp[i].first << " " << dp[i].second << endl;
    // }
    cout << sizeOfCommittee % modulo << " " << numOfWays % modulo;
}
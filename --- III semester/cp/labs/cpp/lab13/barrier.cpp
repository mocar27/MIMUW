#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>

#include "log.hpp"

using namespace std;

int const N_THREADS = 4;

class Barrier {
private:
    int count;
    int resistance;
    mutex m;
    condition_variable cv;

public:
    Barrier(int n): count(0), resistance(n) {}

    void reach() {
        unique_lock<mutex> lock(m);
        count++;
        if (count >= resistance) {
            log("Thread ", this_thread::get_id(), " doesn't stop at the barrier");
            cv.notify_all();
        }
        else {
            cv.wait(lock, [this] {return count >= resistance;});
        }
    }
};

int main() {
    Barrier b(N_THREADS);
    thread t[N_THREADS];
    for (int i = 0; i < N_THREADS; i++) {
        t[i] = thread([&b] {
            log("Thread ", this_thread::get_id(), " is waiting at the barrier");
        b.reach();
        log("Thread ", this_thread::get_id(), " has passed the barrier");
            });
    }

    for (int i = 0; i < N_THREADS; i++) {
        t[i].join();
    }

    return 0;
}
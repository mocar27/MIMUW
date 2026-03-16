#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <exception>
#include <vector>
#include <unordered_map>
#include <functional>
#include <future>
#include <thread>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <set>

#include "machine.hpp"

using namespace std;

class FulfillmentFailure: public exception {
};

class OrderNotReadyException: public exception {
};

class BadOrderException: public exception {
};

class BadPagerException: public exception {
};

class OrderExpiredException: public exception {
};

class RestaurantClosedException: public exception {
};

struct WorkerReport {
    std::vector<std::vector<std::string>> collectedOrders;
    std::vector<std::vector<std::string>> abandonedOrders;
    std::vector<std::vector<std::string>> failedOrders;
    std::vector<std::string> failedProducts;
};

class MachineWrapper {
private:
    shared_ptr<Machine> machine;
    queue<reference_wrapper<promise<unique_ptr<Product>>>> products_queue;
public:
    MachineWrapper(shared_ptr<Machine> machine_):
        machine(machine_) {
    };

    void addProductToQueue(promise<unique_ptr<Product>>& product_promise) {
        products_queue.push(product_promise);
    }

    friend void machineMain(atomic<bool>& working, atomic<unsigned int>& left, MachineWrapper& machine_wrapper);
};

class CoasterPager {
private:
    unsigned int order_id;
    mutex& mtx;
    condition_variable& cv;
    atomic<int>& status; // -1 - not ready, 0 - ready, 1 - fulfilment failure, 2 - expired 

    CoasterPager(unsigned int order_id_, mutex& mtx_, condition_variable& cv_, atomic<int>& status_):
        order_id(order_id_),
        mtx(mtx_),
        cv(cv_),
        status(status_) {
    };

    friend class System;
public:
    void wait() const;

    void wait(unsigned int timeout) const;

    [[nodiscard]] unsigned int getId() const;

    [[nodiscard]] bool isReady() const;
};

class System {
private:
    unordered_map<string, shared_ptr<Machine>> machines;
    unordered_map<shared_ptr<Machine>, thread> machines_threads;
    unordered_map<shared_ptr<Machine>, MachineWrapper> machines_wrappers;
    mutable mutex machines_mutex;
    unsigned int workers_amount;
    unsigned int timeout;

    set<string> current_menu;
    mutable mutex menu_mutex;

    queue<pair<unsigned int, vector<string>>> orders_queue; // queue to take orders from clients in the correct order 
    mutable mutex orders_queue_mutex;
    condition_variable orders_queue_cond;

    unordered_map<unsigned int, atomic<int>> orders; // order number and it's status
    mutable mutex orders_mutex;

    unordered_map<unsigned int, atomic<bool>> is_order_collected; // whether order was collected by client
    unordered_map<unsigned int, vector<unique_ptr<Product>>> orders_ready; // orders ready to be collected by client
    unordered_map<unsigned int, mutex> orders_ready_mutex; // worker obtains mutex while doing the order
    unordered_map<unsigned int, condition_variable> orders_ready_cond; // conditionals to wait for order to be ready

    atomic<unsigned int> orders_counter; // orders counter
    atomic<unsigned int> orders_left; // amount of orders left to do after calling shutdown

    vector<thread> workers;
    vector<promise<WorkerReport>> workers_promises;
    vector<future<WorkerReport>> workers_futures;

    atomic<bool> is_working;

    friend void workerMain(System* system, promise<WorkerReport>& p);

    friend void machineMain(atomic<bool>& working, atomic<unsigned int>& left, MachineWrapper& machine_wrapper);
public:
    typedef std::unordered_map<std::string, std::shared_ptr<Machine>> machines_t;

    System(machines_t machines, unsigned int numberOfWorkers, unsigned int clientTimeout);

    std::vector<WorkerReport> shutdown();

    std::vector<std::string> getMenu() const;

    std::vector<unsigned int> getPendingOrders() const;

    std::unique_ptr<CoasterPager> order(std::vector<std::string> products);

    std::vector<std::unique_ptr<Product>> collectOrder(std::unique_ptr<CoasterPager> CoasterPager);

    unsigned int getClientTimeout() const;
};

#endif // SYSTEM_HPP
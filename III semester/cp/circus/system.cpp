#include "system.hpp"

#include <string>
#include <chrono>
#include <tuple>

using namespace std;

typedef unordered_map<string, shared_ptr<Machine>> machines_t;

void machineMain(atomic<bool>& working, atomic<unsigned int>& left, MachineWrapper& machine_wrapper) {

    while (working.load() || left.load() > 0 || !machine_wrapper.products_queue.empty()) {

        if (!machine_wrapper.products_queue.empty()) {

            auto product_promise = machine_wrapper.products_queue.front();
            machine_wrapper.products_queue.pop();

            try {
                product_promise.get().set_value(machine_wrapper.machine->getProduct());
            }
            catch (MachineFailure& e) {
                product_promise.get().set_exception(current_exception());
            }
        }
    }
}

void workerMain(System* system, promise<WorkerReport>& p) {

    struct WorkerReport wr;
    vector<unique_ptr<Product>> products;
    vector<promise<unique_ptr<Product>>> products_promises;
    vector<future<unique_ptr<Product>>> products_futures;

    while (system->is_working.load() || system->orders_left.load() > 0) {

        unique_lock<mutex> queue_lock(system->orders_queue_mutex);

        system->orders_queue_cond.wait(queue_lock, [&system]() {
            return (system->orders_left > 0 || !system->is_working.load());
            });

        if (!system->is_working.load() && system->orders_left.load() == 0) {
            queue_lock.unlock();
            break;
        }

        unsigned int order_id = system->orders_queue.front().first;
        vector<string> order = system->orders_queue.front().second;
        system->orders_queue.pop();

        products.clear();
        products_promises.clear();
        products_futures.clear();

        products.resize(order.size());
        products_promises.resize(order.size());
        products_futures.resize(order.size());

        for (unsigned int i = 0; i < order.size(); i++) {
            products_futures[i] = products_promises[i].get_future();
        }

        for (unsigned int i = 0; i < order.size(); i++) {
            auto machine = system->machines[order[i]];
            system->machines_wrappers.at(machine).addProductToQueue(ref(products_promises[i]));
        }

        system->orders_left--;
        queue_lock.unlock();
        unique_lock<mutex> lock(system->orders_ready_mutex[order_id]);

        bool did_fail = false;

        for (unsigned int i = 0; i < order.size(); i++) {
            try {
                products_futures[i].wait();
                products[i] = move(products_futures[i].get());
            }
            catch (MachineFailure& e) {
                if (!did_fail) {
                    did_fail = true;

                    system->orders_mutex.lock();
                    system->orders.at(order_id).store(1);
                    system->orders_mutex.unlock();

                    system->orders_ready_cond[order_id].notify_one();
                    lock.unlock();
                }

                wr.failedProducts.push_back(order[i]);

                system->menu_mutex.lock();
                system->current_menu.erase(order[i]);
                system->menu_mutex.unlock();
            }
        }

        if (did_fail) {
            for (unsigned int i = 0; i < order.size(); i++) {
                if (products[i] != nullptr) {
                    system->machines[order[i]]->returnProduct(move(products[i]));
                }
            }
            wr.failedOrders.push_back(order);
        }
        else {

            system->orders_ready.insert({ order_id, move(products) });

            system->orders_mutex.lock();
            system->orders.at(order_id).store(0);
            system->orders_mutex.unlock();

            system->orders_ready_cond[order_id].notify_one();

            auto cur_time = chrono::system_clock::now();
            system->orders_ready_cond[order_id].wait_until(
                lock,
                cur_time + chrono::milliseconds(system->timeout),
                [&] {
                    return system->is_order_collected.at(order_id).load();
                });


            if (!system->is_order_collected.at(order_id).load()) {
                products = move(system->orders_ready[order_id]);

                system->orders_mutex.lock();
                system->orders.at(order_id).store(2);
                system->orders_mutex.unlock();

                system->orders_ready_cond[order_id].notify_one();

                for (unsigned int i = 0; i < order.size(); i++) {
                    if (products[i] != nullptr) {
                        system->machines[order[i]]->returnProduct(move(products[i]));
                    }
                }

                lock.unlock();
                wr.abandonedOrders.push_back(order);
            }
            else {
                lock.unlock();
                wr.collectedOrders.push_back(order);
            }
        }
    // }
    }
    p.set_value(wr);
}

void CoasterPager::wait() const {

    unique_lock<mutex> lock(mtx);
    cv.wait(lock, [this] {return status.load() != -1; });

    if (status.load() == 1) {
        throw FulfillmentFailure();
    }
}

void CoasterPager::wait(unsigned int timeout) const {

    mutex local_mutex;
    unique_lock<mutex> lock(local_mutex);
    auto cur_time = chrono::system_clock::now();
    cv.wait_until(lock, cur_time + chrono::milliseconds(timeout), [this] {return status.load() != -1; });

    if (status.load() == 1) {
        throw FulfillmentFailure();
    }
}

unsigned int CoasterPager::getId() const {
    return CoasterPager::order_id;
}

bool CoasterPager::isReady() const {
    return CoasterPager::status.load() == 0;
}

System::System(machines_t machines, unsigned int numberOfWorkers, unsigned int clientTimeout) {

    this->machines = machines;
    this->workers_amount = numberOfWorkers;
    this->timeout = clientTimeout;
    orders_counter = 0;
    orders_left = 0;

    is_working.store(true);

    for (auto& [name, machine] : machines) {
        machine->start();
        machines_wrappers.emplace(machine, MachineWrapper(machine));
        machines_threads[machine] = thread{
            machineMain,
            ref(is_working),
            ref(orders_left),
            ref(machines_wrappers.at(machine)) };

        current_menu.insert(name);
    }

    workers_futures.resize(numberOfWorkers);
    workers.resize(numberOfWorkers);
    workers_promises.resize(numberOfWorkers);

    for (unsigned int i = 0; i < numberOfWorkers; i++) {
        workers_futures[i] = workers_promises[i].get_future();
        workers[i] = thread{ workerMain, this, ref(workers_promises[i]) };
    }
}

vector<WorkerReport> System::shutdown() {

    is_working.store(false, memory_order_seq_cst);
    vector<WorkerReport> shutdown_reports(workers_amount);
    orders_queue_cond.notify_all();

    for (unsigned int i = 0; i < workers_amount; i++) {
        workers[i].join();
        shutdown_reports[i] = workers_futures[i].get();
    }

    for (auto& [machine, machine_thread] : machines_threads) {
        machine_thread.join();
        machine->stop();
    }

    return shutdown_reports;
}

vector<string> System::getMenu() const {

    vector<string> menu;

    if (!is_working.load()) {
        return menu;
    }

    unique_lock<mutex> lock(menu_mutex);
    for (auto& name : current_menu) {
        menu.emplace_back(name);
    }

    return menu;
}

vector<unsigned int> System::getPendingOrders() const {

    lock_guard<mutex> lock(orders_mutex);

    vector<unsigned int> pending_orders_ids;
    for (auto& [id, status] : orders) {
        atomic<int> temp = status.load();
        if (temp == -1 || temp == 0) {
            pending_orders_ids.emplace_back(id);
        }
    }

    return pending_orders_ids;
}

unique_ptr<CoasterPager> System::order(vector<string> order) {

    if (!is_working.load(memory_order_seq_cst)) {
        throw RestaurantClosedException();
    }
    if (order.empty()) {
        throw BadOrderException();
    }

    menu_mutex.lock();
    for (auto& name : order) {
        if (!current_menu.count(name)) {
            throw BadOrderException();
        }
    }
    menu_mutex.unlock();

    lock_guard<mutex> lock(orders_queue_mutex);

    orders_counter++;
    orders_left++;

    orders_queue.push({ orders_counter - 1, order });
    orders_queue_cond.notify_one();

    orders_mutex.lock();
    orders[orders_counter - 1].store(-1, memory_order_seq_cst);
    orders_mutex.unlock();

    orders_ready_mutex.emplace(piecewise_construct, forward_as_tuple(orders_counter - 1), forward_as_tuple());
    is_order_collected[orders_counter - 1].store(false, memory_order_seq_cst);

    auto temp_pager = CoasterPager(
        orders_counter - 1,
        ref(orders_ready_mutex[orders_counter - 1]),
        ref(orders_ready_cond[orders_counter - 1]),
        ref(orders[orders_counter - 1]
        ));

    return make_unique<CoasterPager>(move(temp_pager));
}

vector<unique_ptr<Product>> System::collectOrder(unique_ptr<CoasterPager> CoasterPager) {

    if (CoasterPager->status.load() == -1) {
        throw OrderNotReadyException();
    }
    else if (CoasterPager->status.load() == 1) {
        throw FulfillmentFailure();
    }
    else {
        lock_guard<mutex> lock(CoasterPager->mtx);

        if (CoasterPager->status.load() == 1) {
            throw FulfillmentFailure();
        }
        else if (CoasterPager->status.load() == 2) {
            throw OrderExpiredException();
        }
        else if (is_order_collected[CoasterPager->order_id].load()) {
            throw BadPagerException();
        }

        is_order_collected[CoasterPager->order_id].store(true, memory_order_seq_cst);
        CoasterPager->cv.notify_one();
        return move(orders_ready[CoasterPager->order_id]);
    }
}

unsigned int System::getClientTimeout() const {
    return System::timeout;
}
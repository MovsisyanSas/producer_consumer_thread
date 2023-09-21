#include <iostream>
#include <stack>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

const int s = 10; // Buffer's max size
std::stack<int> buf;
int products = 10000000;
std::condition_variable not_full;
std::condition_variable not_empty;
std::mutex mtx;

void prod_func();
void cons_func();

int main() {
    auto start_time = std::chrono::high_resolution_clock::now();

    std::thread producer(prod_func);
    std::thread consumer(cons_func);

    producer.join();
    consumer.join();

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::cout << "Runtime duration: " << duration.count() << " microseconds" << std::endl;

    return 0;
}

void prod_func() {
    for (int i = 0; i < products; i++) {
        std::unique_lock<std::mutex> lock(mtx);

        not_full.wait(lock, []() { return buf.size() < s; });
        buf.push(i);
        not_empty.notify_all();

        lock.unlock();
    }
}

void cons_func() {
    for (int i = 0; i < products; i++) {
        std::unique_lock<std::mutex> lock(mtx);

        not_empty.wait(lock, []() { return !buf.empty(); });
        buf.pop();
        not_full.notify_all();

        lock.unlock();
    }
}

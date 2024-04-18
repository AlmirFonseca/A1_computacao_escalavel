#include <iostream>
#include <thread>
#include "../src/Semaphore.hpp"

void worker(int id, Semaphore& semaphore) {
    std::cout << "Worker " << id << " is waiting." << std::endl;
    semaphore.wait();
    std::cout << "Worker " << id << " has acquired the semaphore." << std::endl;
    // Simulate some work
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Worker " << id << " is releasing the semaphore." << std::endl;
    semaphore.notify();
}

int main() {
    Semaphore semaphore(2); // Create a semaphore with a maximum count of 2

    // Create worker threads
    std::thread worker1(worker, 1, std::ref(semaphore));
    std::thread worker2(worker, 2, std::ref(semaphore));
    std::thread worker3(worker, 3, std::ref(semaphore));

    // Wait for all worker threads to finish
    worker1.join();
    worker2.join();
    worker3.join();

    return 0;
}
#include "../src/ThreadPool.hpp"
#include <iostream>

using namespace std;

void task0(int id) {
    printf("Task0 with id %d started\n", id);
    this_thread::sleep_for(chrono::seconds(1));
    printf("Task0 with id %d finished\n", id);
}

void task1() {
    printf("Task 1 started\n");
    this_thread::sleep_for(chrono::seconds(1));
    printf("Task 1 finished\n");
}


int main() {
    // Create a thread pool with 4 threads
    ThreadPool pool(4);

    // Add tasks to the thread pool
    pool.addTask(task0, 0);
    pool.addTask(task0, 1);
    pool.addTask(task1);
    pool.addTask(task0, 2);

    return 0;
}
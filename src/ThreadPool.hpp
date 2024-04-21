#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <functional>

using namespace std;


/**
 * @brief A thread pool class  
 * 
 * The ThreadPool class creates a number of threads and allows tasks to be added to a queue. 
 * The threads will execute the tasks in the queue until the ThreadPool object is destroyed.
 */
class ThreadPool {
public:

    /**
     * @brief Construct a new ThreadPool object
     * 
     * @param numThreads The number of threads to be created
     */
    ThreadPool(int numThreads) : numThreads(numThreads) {
        // Create a number of threads and start them
        for (int i = 0; i < numThreads; i++) {
            threads.push_back(thread([this] { this->run(); }));
        }
    }

    /**
     * @brief Destroy the ThreadPool object
     * 
     * Safely stop the threads and wait for them to finish
     */
    ~ThreadPool() {
        {
            // Safely stop the threads
            unique_lock<mutex> lock(queueMutex);
            stop = true;
        }

        condition.notify_all();

        // Wait for the threads to finish
        for (auto& thread : threads) {
            thread.join();
        }
    }

    /**
     * @brief Add a task with certain arguments to the thread pool
     * 
     * @param task The task to be added
     * @param args The arguments to be passed to the task
     */
    template<class F, class... Args>
    void addTask(F&& task, Args&&... args) {
        {
            // Add the task to the queue
            unique_lock<mutex> lock(queueMutex);
            tasks.push_back([=] { task(args...); });
        }

        // Notify a thread that there is a task to be executed
        condition.notify_one();
    }

private:
    /**
     * @brief The function that the threads will execute
     */
    void run() {
        while (true) {
            function<void()> task;
            {
                unique_lock<mutex> lock(queueMutex);
                
                // Block the thread until there is a task in the queue or the thread is stopped
                condition.wait(lock, [this] { return !tasks.empty() || stop; });

                // If the thread is stopped and the queue is empty, return
                if (stop && tasks.empty()) {
                    return;
                }

                // Get the task from the top of the queue
                task = pop_and_push_task();
            }

            // Execute the task
            task();
        }
    }

    /**
     * @brief Remove the task from the top of the queue and add it to the bottom of the queue
     * 
     * @return The task that was removed
     */
    function<void()> pop_and_push_task() {
        // Get the task from the top of the queue
        auto task = tasks.front();

        // Remove the task from the top of the queue
        tasks.pop_front();

        // Add the task to the bottom of the queue
        tasks.push_back([=] { task(); });

        return task;
    }

    int numThreads; // Number of threads
    vector<thread> threads; // Vector of threads
    deque<function<void()>> tasks; // Queue of tasks
    mutex queueMutex; // Mutex for the queue
    condition_variable condition; // Condition variable for the queue
    bool stop = false; // Flag to stop the threads
};

#endif
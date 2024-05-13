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
        printf("Number of threads: %d\n", numThreads);
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
            numTasks++;
        }

        // Notify a thread that there is a task to be executed
        condition.notify_one();
    }

private:
    /**
     * @brief The function that the threads will execute
     */
    void run() { 
        // Wait for a task to be added to the queue
        while (numTasks == 0) 
        {
            this_thread::sleep_for(chrono::milliseconds(100));
        }

        // Execute the tasks in the queue
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

                // Get the task according to the taskIndex
                task = get_next_task();

                // Increment the taskIndex
                taskIndex++;
            }

            // Execute the task
            task();

            // Notify the condition variable that the task is finished
            condition.notify_one();
        }
    }

    /**
     * @brief Get the next task from the queue using the taskIndex
     * 
     * @return The task to be executed
     */
    function<void()> get_next_task() {
        // Get the task from the taskIndex
        function<void()> task = tasks[taskIndex %= numTasks];
        
        return task;
    }

    int numThreads; // Number of threads
    int numTasks = 0; // Number of tasks
    int taskIndex = 0; // Index of the task
    vector<thread> threads; // Vector of threads
    deque<function<void()>> tasks; // Queue of tasks
    mutex queueMutex; // Mutex for the queue
    condition_variable condition; // Condition variable for the queue
    bool stop = false; // Flag to stop the threads
};

#endif
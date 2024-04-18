# ifndef SEMAPHORE_HPP
# define SEMAPHORE_HPP

#include <iostream>
#include <mutex>
#include <condition_variable>

/**
 * @brief Class for handling a semaphore.
 */

class Semaphore {
private:
    int count;
    int max_count;
    std::mutex mutex;
    std::condition_variable condition;

public:
    /**
     * @brief Construct a new Semaphore object.
     * 
     * @param count The initial count of the semaphore.
     */
    Semaphore(int count = 0, int max_count = 1) : count(count), max_count(max_count) {}

    virtual ~Semaphore() = default;

    /**
     * @brief Wait for the semaphore.
     */
    void wait() {
        std::unique_lock<std::mutex> lock(mutex);
        while (count == 0) {
            condition.wait(lock);
        }
        count--;
    }

    /**
     * @brief Notify the semaphore.
     */
    void notify() {
        std::unique_lock<std::mutex> lock(mutex);
        while (count == max_count) {
            condition.wait(lock);
        }
        count++;
        condition.notify_one();
    }
};

# endif // SEMAPHORE_HPP
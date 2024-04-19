#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <iostream>
#include <queue>
#include <mutex>

#include "Semaphore.hpp"

/**
 * @brief Class for handling a queue of data.
 */
template <typename T>
class Queue {
private:
    std::queue<T> queue;
    Semaphore emptyQueue;
    Semaphore fullQueue;
    std::mutex mutex;

public:
    Queue(int size) : emptyQueue(size), fullQueue(0) {}

    /**
     * @brief Pushes an element to the queue.
     * 
     * @param element Element to be pushed.
     */
    void push(T element) {
        emptyQueue.wait();
        mutex.lock();
        queue.push(element);
        mutex.unlock();
        fullQueue.notify();
    }

    /**
     * @brief Pops an element from the queue.
     * 
     * @return T Popped element.
     */
    T pop() {
        fullQueue.wait();
        mutex.lock();
        T element = queue.front();
        queue.pop();
        mutex.unlock();
        emptyQueue.notify();
        return element;
    }

    /**
     * @brief Checks if the queue is empty.
     * 
     * @return true If the queue is empty.
     * @return false If the queue is not empty.
     */
    bool isEmpty() {
        return queue.empty();
    }

    /**
     * @brief Returns the size of the queue.
     * 
     * @return int Size of the queue.
     */
    int size() {
        return queue.size();
    }

    /**
     * @brief Prints the queue.
     */
    void print() {
        std::queue<T> temp = queue;
        while (!temp.empty()) {
            std::cout << temp.front() << " ";
            temp.pop();
        }
        std::cout << std::endl;
    }

    /**
     * @brief Destructor for the Queue object.
     */
    ~Queue() {}
};

#endif // QUEUE_HPP
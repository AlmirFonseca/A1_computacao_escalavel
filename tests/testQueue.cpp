#include <iostream>
#include "../src/Queue.hpp"

int main() {
    // Create a queue of integers
    Queue<int> intQueue(3);

    // Push elements to the queue
    intQueue.push(10);
    intQueue.push(20);
    intQueue.push(30);

    // Print the queue
    std::cout << "Queue: ";
    intQueue.print(); // Output: 10 20 30

    // Pop an element from the queue
    int poppedElement = intQueue.pop();
    std::cout << "Popped Element: " << poppedElement << std::endl; // Output: Popped Element: 10

    // Check if the queue is empty
    bool isEmpty = intQueue.isEmpty();
    std::cout << "Is Empty: " << (isEmpty ? "true" : "false") << std::endl; // Output: Is Empty: false

    // Get the size of the queue
    int queueSize = intQueue.size();
    std::cout << "Queue Size: " << queueSize << std::endl; // Output: Queue Size: 2

    return 0;
}
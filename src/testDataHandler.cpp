#include <iostream>
#include "DataHandler.hpp"

// Example of using the data handler in a separate thread
int main() {
    // Creation of input and output queues
    std::queue<std::string> inputQueue;
    std::queue<std::string> outputQueue;

    // Creation of the specific data handler
    std::shared_ptr<DataHandler> handler = std::make_shared<DataHandler>(inputQueue, outputQueue);

    // Start the execution of the data handler in a separate thread
    std::thread handlerThread(&DataHandler::start, handler);

    // Example of inserting data into the input queue
    inputQueue.push("10");

    // Wait for a while to allow the data handler to process the data
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Display the processed data from the output queue
    while (!outputQueue.empty()) {
        std::cout << "Processed data: " << outputQueue.front() << std::endl;
        outputQueue.pop();
    }

    // Stop the thread of the data handler
    handlerThread.join();

    return 0;
}
#ifndef DATAHANDLER_HPP
#define DATAHANDLER_HPP

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
#include <string>

/**
 * @brief Class for handling data in a separate thread.
 */
class DataHandler {
public:
    /**
     * @brief Construct a new DataHandler object.
     * 
     * @param input_queue Reference to the input queue.
     * @param output_queue Reference to the output queue.
     */
    DataHandler(std::queue<std::string>& input_queue, std::queue<std::string>& output_queue)
        : input_queue(input_queue), output_queue(output_queue), stop_flag(false) {}

    /**
     * @brief Start the data handling thread.
     */
    void start() {
        thread = std::thread(&DataHandler::run, this);
    }

    /**
     * @brief Stop the data handling thread.
     */
    void stop() {
        {
            std::unique_lock<std::mutex> lock(mutex);
            stop_flag = true;
        }
        condition.notify_all(); // Notifies the thread to stop
        thread.join(); // Wait for the thread to finish
    }

    /**
     * @brief Process the data.
     * 
     * @param data The data to be processed.
     */
    void process_data(std::string& data) {
        // Data processing logic
        std::string processed_data = "Processed: " + data;
        std::cout << "Processed data: " << processed_data << std::endl;
        // Put the processed data into the output queue
        {
            std::unique_lock<std::mutex> lock(mutex);
            output_queue.push(processed_data);
        }
        condition.notify_one(); // Notifies the thread to proceed
    }

private:
    /**
     * @brief Main function to run in the thread.
     */
    void run() {
        while (true) {
            std::string data;
            {
                std::unique_lock<std::mutex> lock(mutex);
                // Wait until there is data in the input queue or until the thread is stopped
                condition.wait(lock, [&] { return !input_queue.empty() || stop_flag; });
                // If the thread is stopped and the input queue is empty, break the loop
                if (stop_flag && input_queue.empty()) break;
                // Get the data from the input queue
                data = input_queue.front();
                input_queue.pop();
            }
            // Process the data
            process_data(data);
        }
    }

    std::thread thread; /**< The thread object */
    std::queue<std::string>& input_queue; /**< Reference to the input queue */
    std::queue<std::string>& output_queue; /**< Reference to the output queue */
    std::mutex mutex; /**< Mutex for thread synchronization */
    std::condition_variable condition; /**< Condition variable for thread synchronization */
    bool stop_flag; /**< Flag to indicate if the thread should stop */
};

#endif // DATAHANDLER_HPP
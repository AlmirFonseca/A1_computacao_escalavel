#ifndef DATAHANDLER_HPP
#define DATAHANDLER_HPP

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
#include <string>
#include "DataFrame.hpp"

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

    virtual void handle(DataFrame& df) = 0;

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

/**
 * @brief Class for filtering data in a DataFrame.
 */
class FilterHandler : public DataHandler {
public:
    /**
     * @brief Construct a new FilterHandler object.
     * 
     * @param input_queue Reference to the input queue.
     * @param output_queue Reference to the output queue.
     */
    FilterHandler(std::queue<std::string>& input_queue, std::queue<std::string>& output_queue)
        : DataHandler(input_queue, output_queue) {}

    /**
     * @brief Handle the data.
     * 
     * @param df The DataFrame to be filtered.
     */
    void handle(std::string& columnName, std::string& operation, std::string& value) {
        // Implement the data filtering logic
        // Filter the data if the column type is int
        if (columnName == typeid(int).name()) {
            // Get the column data
            std::vector<int> columnData = df.getColumn<int>(columnName);
            // Create a new vector to store the filtered data
            std::vector<int> filteredData;
            // Filter the data based on the operation
            // Filter for equality
            if (operation == "==") {
                // Loop through the column data
                for (int i = 0; i < columnData.size(); i++) {
                    // If the value in the column is equal to the filter value, 
                    // add it to the filtered data
                    if (columnData[i] == std::stoi(value)) {
                        filteredData.push_back(columnData[i]);
                    }
                }
            // Filter for inequality
            } else if (operation == "!=") {
                for (int i = 0; i < columnData.size(); i++) {
                    // If the value in the column is not equal to the filter value,
                    // add it to the filtered data
                    if (columnData[i] != std::stoi(value)) {
                        filteredData.push_back(columnData[i]);
                    }
                }
            // Filter for greater than
            } else if (operation == ">") {
                for (int i = 0; i < columnData.size(); i++) {
                    // If the value in the column is greater than the filter value,
                    // add it to the filtered data
                    if (columnData[i] > std::stoi(value)) {
                        filteredData.push_back(columnData[i]);
                    }
                }
            // Filter for less than
            } else if (operation == "<") {
                for (int i = 0; i < columnData.size(); i++) {
                    // If the value in the column is less than the filter value,
                    // add it to the filtered data
                    if (columnData[i] < std::stoi(value)) {
                        filteredData.push_back(columnData[i]);
                    }
                }
            // Filter for greater than or equal to
            } else if (operation == ">=") {
                for (int i = 0; i < columnData.size(); i++) {
                    // If the value in the column is greater than or equal to the
                    // filter value, add it to the filtered data
                    if (columnData[i] >= std::stoi(value)) {
                        filteredData.push_back(columnData[i]);
                    }
                }
            // Filter for less than or equal to
            } else if (operation == "<=") {
                for (int i = 0; i < columnData.size(); i++) {
                    // If the value in the column is less than or equal to the
                    // filter value, add it to the filtered data
                    if (columnData[i] <= std::stoi(value)) {
                        filteredData.push_back(columnData[i]);
                    }
                }
            // If the operation is not recognized, throw an exception
            } else {
                throw std::invalid_argument("Invalid operation");
            }
            // Add the filtered data to the DataFrame
            df.addColumn<int>(columnName, filteredData);
        }
        // Filter the data if the column type is double
        else if (columnName == typeid(double).name()) {
            // Get the column data
            std::vector<double> columnData = df.getColumn<double>(columnName);
            // Create a new vector to store the filtered data
            std::vector<double> filteredData;
            // Filter the data based on the operation
            // Filter for equality
            if (operation == "==") {
                for (int i = 0; i < columnData.size(); i++) {
                    // If the value in the column is equal to the filter value, 
                    // add it to the filtered data
                    if (columnData[i] == std::stod(value)) {
                        filteredData.push_back(columnData[i]);
                    }
                }
            // Filter for inequality
            } else if (operation == "!=") {
                for (int i = 0; i < columnData.size(); i++) {
                    // If the value in the column is not equal to the filter value,
                    // add it to the filtered data
                    if (columnData[i] != std::stod(value)) {
                        filteredData.push_back(columnData[i]);
                    }
                }
            // Filter for greater than
            } else if (operation == ">") {
                for (int i = 0; i < columnData.size(); i++) {
                    // If the value in the column is greater than the filter value,
                    // add it to the filtered data
                    if (columnData[i] > std::stod(value)) {
                        filteredData.push_back(columnData[i]);
                    }
                }
            // Filter for less than
            } else if (operation == "<") {
                for (int i = 0; i < columnData.size(); i++) {
                    // If the value in the column is less than the filter value,
                    // add it to the filtered data
                    if (columnData[i] < std::stod(value)) {
                        filteredData.push_back(columnData[i]);
                    }
                }
            // Filter for greater than or equal to
            } else if (operation == ">=") {
                for (int i = 0; i < columnData.size(); i++) {
                    // If the value in the column is greater than or equal to the
                    // filter value, add it to the filtered data
                    if (columnData[i] >= std::stod(value)) {
                        filteredData.push_back(columnData[i]);
                    }
                }
            // Filter for less than or equal to
            } else if (operation == "<=") {
                for (int i = 0; i < columnData.size(); i++) {
                    // If the value in the column is less than or equal to the
                    // filter value, add it to the filtered data
                    if (columnData[i] <= std::stod(value)) {
                        filteredData.push_back(columnData[i]);
                    }
                }
            // If the operation is not recognized, throw an exception
            } else {
                throw std::invalid_argument("Invalid operation");
            }
            // Add the filtered data to the DataFrame
            df.addColumn<double>(columnName, filteredData);
        } 
        // Filter the data if the column type is string
        else if (columnName == typeid(std::string).name()) {
            // Get the column data
            std::vector<std::string> columnData = df.getColumn<std::string>(columnName);
            // Create a new vector to store the filtered data
            std::vector<std::string> filteredData;
            // Filter the data based on the operation
            // Filter for equality
            if (operation == "==") {
                for (int i = 0; i < columnData.size(); i++) {
                    // If the value in the column is equal to the filter value, 
                    // add it to the filtered data
                    if (columnData[i] == value) {
                        filteredData.push_back(columnData[i]);
                    }
                }
            // Filter for inequality
            } else if (operation == "!=") {
                for (int i = 0; i < columnData.size(); i++) {
                    // If the value in the column is not equal to the filter value,
                    // add it to the filtered data
                    if (columnData[i] != value) {
                        filteredData.push_back(columnData[i]);
                    }
                }
            // If the operation is not recognized, throw an exception
            } else {
                throw std::invalid_argument("Invalid operation");
            }
            // Add the filtered data to the DataFrame
            df.addColumn<std::string>(columnName, filteredData);
        } 
        // Filter the data if the column type is char
        else if (columnName == typeid(char).name()) {
            // Get the column data
            std::vector<char> columnData = df.getColumn<char>(columnName);
            // Create a new vector to store the filtered data
            std::vector<char> filteredData;
            // Filter the data based on the operation
            // Filter for equality
            if (operation == "==") {
                for (int i = 0; i < columnData.size(); i++) {
                    // If the value in the column is equal to the filter value,
                    // add it to the filtered data
                    if (columnData[i] == value[0]) {
                        filteredData.push_back(columnData[i]);
                    }
                }
            // Filter for inequality
            } else if (operation == "!=") {
                for (int i = 0; i < columnData.size(); i++) {
                    // If the value in the column is not equal to the filter value,
                    // add it to the filtered data
                    if (columnData[i] != value[0]) {
                        filteredData.push_back(columnData[i]);
                    }
                }
            // If the operation is not recognized, throw an exception
            } else {
                throw std::invalid_argument("Invalid operation");
            }
            // Add the filtered data to the DataFrame
            df.addColumn<char>(columnName, filteredData);
        } 
        // If the column type is not recognized, throw an exception
        else {
            throw std::invalid_argument("Invalid column type");
        }
        // Put the DataFrame into the output queue
        std::unique_lock<std::mutex> lock(mutex);
        output_queue.push(df.toString());
    }

#endif // DATAHANDLER_HPP
#ifndef DATAHANDLER_HPP
#define DATAHANDLER_HPP

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <string>
#include "DataFrame.hpp"
#include "Queue.hpp"

/**
 * @brief Class for handling data in a separate thread.
 *  
 * This class is a base class for handling data in a separate thread.
 * It reads data from an input queue, processes it and writes the result to an output queue.
 */
class DataHandler {
protected:
    Queue<DataFrame> inputQueue;
    Queue<DataFrame> outputQueue;

public:
    /**
     * @brief Construct a new DataHandler object.
     * 
     * @param inputQueueSize Reference to the input queue.
     * @param outputQueueSize Reference to the output queue.
     */
    // Constructor
    DataHandler(int inputQueueSize, int outputQueueSize)
        : inputQueue(inputQueueSize), outputQueue(outputQueueSize) {}

    // Destructor
    virtual ~DataHandler() {}

    /**
     * @brief Execute the data handler.
     * 
     * This method is called by the thread to execute the data handler.
     * It reads data from the input queue, processes it and writes the result to the output queue.
     */
    void operator()() {
        while (true) {
            // Read data from the input queue
            DataFrame df = inputQueue.pop();

            // Process the data
            execute(df);

            // Write the result to the output queue
            outputQueue.push(df);
        }
    }

    /**
     * @brief Execute the data handler.
     * 
     * This method is called by the thread to execute the data handler.
     * It processes the data in a DataFrame.
     * 
     * @param df The DataFrame to process.
     */
    virtual void execute(DataFrame& df) = 0;
};

/**
 * @brief Class for filtering data in a DataFrame.
 * 
 * This class is a subclass of DataHandler.
 * It filters the data in a DataFrame based on a column name and a filter value.
 * The rows that match the filter can be kept or removed.
 */
class FilterHandler : public DataHandler {
private:
    std::string ColunmName;
    std::any filterValue;
    bool keep;

public:
    /**
     * @brief Construct a new FilterHandler object.
     * 
     * @param inputQueueSize Reference to the input queue.
     * @param outputQueueSize Reference to the output queue.
     * @param ColunmName The name of the column to filter.
     * @param filterValue The value to filter.
     * @param keep Flag to indicate if the rows that match the filter should be kept.
     */
    FilterHandler(int inputQueueSize, int outputQueueSize, std::string ColunmName, std::any filterValue, bool keep)
        : DataHandler(inputQueueSize, outputQueueSize), ColunmName(ColunmName), filterValue(filterValue), keep(keep) {}

    // Inherited from DataHandler
    void execute(DataFrame& df) override {
        // Filter the DataFrame
        df.filterByColumn(ColunmName, filterValue, keep);
    }
};

/**
 * @brief Class for joining two DataFrames.
 * 
 * This class is a subclass of DataHandler.
 * It joins two DataFrames based on a column name.
 */
class JoinHandler : public DataHandler {
private:
    std::string ColunmName;

public:
    /**
     * @brief Construct a new JoinHandler object.
     * 
     * @param inputQueueSize Reference to the input queue.
     * @param outputQueueSize Reference to the output queue.
     * @param ColunmName The name of the column to join.
     */
    JoinHandler(int inputQueueSize, int outputQueueSize, std::string ColunmName)
        : DataHandler(inputQueueSize, outputQueueSize), ColunmName(ColunmName) {}

    // Inherited from DataHandler
    void execute(DataFrame& df) override {
        // Join the DataFrame
        df.joinByColumn(ColunmName);
    }
};

/**
 * @brief Class for grouping data in a DataFrame.
 * 
 * This class is a subclass of DataHandler.
 * It groups the data in a DataFrame based on a column name.
 */
class GroupByHandler : public DataHandler {
private:
    std::string ColunmName;

public:
    /**
     * @brief Construct a new GroupByHandler object.
     * 
     * @param inputQueueSize Reference to the input queue.
     * @param outputQueueSize Reference to the output queue.
     * @param ColunmName The name of the column to group.
     */
    GroupByHandler(int inputQueueSize, int outputQueueSize, std::string ColunmName)
        : DataHandler(inputQueueSize, outputQueueSize), ColunmName(ColunmName) {}

    // Inherited from DataHandler
    void execute(DataFrame& df) override {
        // Group the DataFrame
        df.groupByColumn(ColunmName);
    }
};

/**
 * @brief Class for sorting data in a DataFrame.
 * 
 * This class is a subclass of DataHandler.
 * It sorts the data in a DataFrame based on a column name.
 */
class SortHandler : public DataHandler {
private:
    std::string ColunmName;

public:
    /**
     * @brief Construct a new SortHandler object.
     * 
     * @param inputQueueSize Reference to the input queue.
     * @param outputQueueSize Reference to the output queue.
     * @param ColunmName The name of the column to sort.
     */
    SortHandler(int inputQueueSize, int outputQueueSize, std::string ColunmName)
        : DataHandler(inputQueueSize, outputQueueSize), ColunmName(ColunmName) {}

    // Inherited from DataHandler
    void execute(DataFrame& df) override {
        // Sort the DataFrame
        df.sortByColumn(ColunmName);
    }
};

#endif // DATAHANDLER_HPP
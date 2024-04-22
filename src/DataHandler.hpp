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
    Queue<DataFrame*> *inputQueue;
    Queue<DataFrame*> *outputQueue;

public:
    /**
     * @brief Construct a new DataHandler object.
     * 
     * @param inputQueue Reference to the input queue.
     * @param outputQueue Reference to the output queue.
     */
    // Constructor
    DataHandler(Queue<DataFrame*> *inputQueue, Queue<DataFrame*> *outputQueue)
        : inputQueue(inputQueue), outputQueue(outputQueue) {}
};

/**
 * @brief Class for filtering data in a DataFrame.
 * 
 * This class is a subclass of DataHandler.
 * It filters the data in a DataFrame based on a column name and a filter value.
 * The rows that match the filter can be kept or removed.
 */
class FilterHandler : public DataHandler {
public:
    /**
     * @brief Construct a new FilterHandler object.
     * 
     * @param inputQueue Reference to the input queue.
     * @param outputQueue Reference to the output queue.
     * @param ColunmName The name of the column to filter.
     * @param filterValue The value to filter.
     * @param op The operation to compare the column value with the filter value.
     */
    FilterHandler(Queue<DataFrame*> *inputQueue, Queue<DataFrame*> *outputQueue)
        : DataHandler(inputQueue, outputQueue) {};

    void filterByColumn(std::string columnName, const std::any& filterValue, CompareOperation op) {
        while(true) {

            if (inputQueue->isEmpty()) {
                break;
            }

            // Read the DataFrame from the input queue
            DataFrame* df = inputQueue->pop();

            // Filter the DataFrame
            df->filterByColumn(columnName, filterValue, op);

            // Write the DataFrame to the output queue
            outputQueue->push(df);
        }
    }
};



/**
 * @brief Class for counting the number of lines in a DataFrame.
 * 
 * This class is a subclass of DataHandler.
 * It counts the number of lines in a DataFrame.
 */
class CountLinesHandler : public DataHandler {
public:
    CountLinesHandler(Queue<DataFrame*> *inputQueue, Queue<DataFrame*> *outputQueue)
        : DataHandler(inputQueue, outputQueue) {};

    void countLines() {
        while(true) {
            if (inputQueue->isEmpty()) {
                break;
            }

            // Read the DataFrame from the input queue
            DataFrame* df = inputQueue->pop();

            // Count the lines in the DataFrame
            int lines = df->getRowCount();

            // Create a new DataFrame with the count
            DataFrame* countDf = new DataFrame({"count"});
            countDf->addRow(lines);

            // Write the DataFrame to the output queue
            outputQueue->push(countDf);
        }
    }
};


/**
 * @brief Class for counting the number of values in a DataFrame.
 * 
 * This class is a subclass of DataHandler.
 * It counts the number of values in a DataFrame.
 */
class ValueCountHandler : public DataHandler {
public:
    ValueCountHandler(Queue<DataFrame*> *inputQueue, Queue<DataFrame*> *outputQueue)
        : DataHandler(inputQueue, outputQueue) {};

    void valueCount(std::string columnName) {
        while(true) {
            if (inputQueue->isEmpty()) {
                break;
            }

            // Read the DataFrame from the input queue
            DataFrame* df = inputQueue->pop();

            // Count the values in the DataFrame
            DataFrame* countDf = new DataFrame();
            *countDf = df->valueCounts(columnName);

            // Write the DataFrame to the output queue
            outputQueue->push(countDf);
        }
    }
};

// /**
//  * @brief Class for joining two DataFrames.
//  * 
//  * This class is a subclass of DataHandler.
//  * It joins two DataFrames based on a column name.
//  */
// class JoinHandler : public DataHandler {
// public:
//     /**
//      * @brief Construct a new JoinHandler object.
//      * 
//      * @param inputQueue Reference to the input queue.
//      * @param outputQueue Reference to the output queue.
//      * @param ColunmName The name of the column to join.
//      */
//     JoinHandler(int inputQueue, int outputQueue, std::string ColunmName)
//         : DataHandler(inputQueue, outputQueue) {}

//     // Inherited from Da
//     void execute(DataFrame& df) override {
//         // Join the DataFrame
//         df.joinByColumn(ColunmName);
//     }
// };

// /**
//  * @brief Class for grouping data in a DataFrame.
//  * 
//  * This class is a subclass of DataHandler.
//  * It groups the data in a DataFrame based on a column name.
//  */
// class GroupByHandler : public DataHandler {
// private:
//     std::string ColunmName;

// public:
//     /**
//      * @brief Construct a new GroupByHandler object.
//      * 
//      * @param inputQueue Reference to the input queue.
//      * @param outputQueue Reference to the output queue.
//      * @param ColunmName The name of the column to group.
//      */
//     GroupByHandler(int inputQueue, int outputQueue, std::string ColunmName)
//         : DataHandler(inputQueue, outputQueue), ColunmName(ColunmName) {}

//     // Inherited from DataHandler
//     void execute(DataFrame& df) override {
//         // Group the DataFrame
//         df.groupByColumn(ColunmName);
//     }
// };

// /**
//  * @brief Class for sorting data in a DataFrame.
//  * 
//  * This class is a subclass of DataHandler.
//  * It sorts the data in a DataFrame based on a column name.
//  */
// class SortHandler : public DataHandler {
// private:
//     std::string ColunmName;

// public:
//     /**
//      * @brief Construct a new SortHandler object.
//      * 
//      * @param inputQueue Reference to the input queue.
//      * @param outputQueue Reference to the output queue.
//      * @param ColunmName The name of the column to sort.
//      */
//     SortHandler(int inputQueue, int outputQueue, std::string ColunmName)
//         : DataHandler(inputQueue, outputQueue), ColunmName(ColunmName) {}

//     // Inherited from DataHandler
//     void execute(DataFrame& df) override {
//         // Sort the DataFrame
//         df.sortByColumn(ColunmName);
//     }
// };

#endif // DATAHANDLER_HPP
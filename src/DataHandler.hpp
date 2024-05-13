#ifndef DATAHANDLER_HPP
#define DATAHANDLER_HPP

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <string>
#include <vector>
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
    std::vector<Queue<DataFrame*>*> outputQueues;

public:
    /**
     * @brief Construct a new DataHandler object.
     * 
     * @param inputQueue Reference to the input queue.
     * @param outputQueues Reference to the output queues.
     */
    // Constructor
    DataHandler(Queue<DataFrame*> *inputQueue, std::vector<Queue<DataFrame*>*> outputQueues)
        : inputQueue(inputQueue), outputQueues(outputQueues) {}

    /**
     * @brief Push a DataFrame to the output queues.
     *
     * Creates a deep copy of the DataFrame and pushes it to the output queues.
     * 
     * @param df The DataFrame to push.
     */
    void pushToOutputQueues(DataFrame* df) {
        for (auto& outputQueue : outputQueues) {
            DataFrame* dfCopy = new DataFrame;
            *dfCopy = DataFrame::deepCopy(*df);
            outputQueue->push(dfCopy);
        }
        delete df;
    }
};

/**
 * @brief Class for copying data in a DataFrame.
 * 
 * This class is a subclass of DataHandler.
 * It copies the data in a DataFrame to multiple output queues.
 */
class CopyHandler : public DataHandler {
public:
    CopyHandler(Queue<DataFrame*> *inputQueue, std::vector<Queue<DataFrame*>*> outputQueues)
        : DataHandler(inputQueue, outputQueues) {};

    void copy() {
        while (!inputQueue->isEmpty()) {
            DataFrame* df = inputQueue->pop();

            // Write the DataFrame to the output queues
            pushToOutputQueues(df);
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
    CountLinesHandler(Queue<DataFrame*> *inputQueue, std::vector<Queue<DataFrame*>*> outputQueues)
        : DataHandler(inputQueue, outputQueues) {};

    void countLines() {
        while (!inputQueue->isEmpty()) {

            // Read the DataFrame from the input queue
            DataFrame* df = inputQueue->pop();

            // Count the lines in the DataFrame
            int lines = df->getRowCount();

            // Delete the DataFrame
            delete df;

            // Create a new DataFrame with the count
            DataFrame* countDf = new DataFrame({"Count"});
            countDf->addRow(lines);

            // Write the DataFrame to the output queues
            pushToOutputQueues(countDf);
        }
    }
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
    FilterHandler(Queue<DataFrame*> *inputQueue, std::vector<Queue<DataFrame*>*> outputQueues)
        : DataHandler(inputQueue, outputQueues) {};

    void filterByColumn(std::string columnName, const std::any& filterValue, CompareOperation op) {
        while (!inputQueue->isEmpty()) {
            // Read the DataFrame from the input queue
            DataFrame* df = inputQueue->pop();

            // Filter the DataFrame
            df->filterByColumn(columnName, filterValue, op);

            // Write the DataFrame to the output queue
            pushToOutputQueues(df);
        }
    }
};

/**
 * @brief Class for value counting in a DataFrame.
 * 
 * This class is a subclass of DataHandler.
 * It counts the number of occurrences of each value in a column of a DataFrame.
 */
class ValueCountHandler : public DataHandler {
public:
    /**
     * @brief Construct a new ValueCountHandler object.
     * 
     * @param inputQueue Reference to the input queue.
     * @param outputQueue Reference to the output queue.
     * @param ColunmName The name of the column to count.
     */
    ValueCountHandler(Queue<DataFrame*> *inputQueue, std::vector<Queue<DataFrame*>*> outputQueues)
        : DataHandler(inputQueue, outputQueues) {};

    void countByColumn(std::string columnName) {
        while (!inputQueue->isEmpty()) {
            // Read the DataFrame from the input queue
            DataFrame* df = inputQueue->pop();

            // Count the values in the DataFrame
            DataFrame* countDf = new DataFrame();
            *countDf = df->valueCounts(columnName);

            // Delete the DataFrame
            delete df;

            // Write the DataFrame to the output queue
            pushToOutputQueues(countDf);
        }
    }
};

/**
 * @brief Class for joining two DataFrames.
 * 
 * This class is a subclass of DataHandler.
 * It joins two DataFrames based on a column name.
 * The two DataFrames must have the same column name.
 */
class JoinHandler : public DataHandler {
public:
    /**
     * @brief Construct a new JoinHandler object.
     * 
     * @param inputQueue Reference to the input queue.
     * @param outputQueue Reference to the output queue.
     * @param dfRight The right DataFrame to join.
     * @param KeyColunmName The name of the column to join.
     * @param dropKeyColumn True if the key column should be dropped, false otherwise.
     */
    JoinHandler(Queue<DataFrame*> *inputQueue, std::vector<Queue<DataFrame*>*> outputQueues)
        : DataHandler(inputQueue, outputQueues) {};

    void join(DataFrame& dfRight, std::string keyColumnName, bool dropKeyColumn=false) {
        while (!inputQueue->isEmpty()) {
            // Read the DataFrame from the input queue
            DataFrame* dfLeft = inputQueue->pop();

            // Join the DataFrames
            DataFrame* dfJoined = new DataFrame();
            *dfJoined = dfLeft->leftJoin(dfRight, keyColumnName, dropKeyColumn);

            // Delete the left DataFrame
            delete dfLeft;


            // Write the DataFrame to the output queue
            pushToOutputQueues(dfJoined);
        }
    }
};

/**
 * @brief Class for sorting data in a DataFrame.
 * 
 * This class is a subclass of DataHandler.
 * It sorts the data in a DataFrame based on a column name.
 * The data can be sorted in ascending or descending order.
 */
class SortHandler : public DataHandler {
public:
    /**
     * @brief Construct a new SortHandler object.
     * 
     * @param inputQueue Reference to the input queue.
     * @param outputQueue Reference to the output queue.
     * @param ColunmName The name of the column to sort.
     * @param ascending True if the data should be sorted in ascending order, false otherwise.
     */
    SortHandler(Queue<DataFrame*> *inputQueue, std::vector<Queue<DataFrame*>*> outputQueues)
        : DataHandler(inputQueue, outputQueues) {};

    void sortByColumn(std::string columnName, bool ascending=true) {
        while (!inputQueue->isEmpty()) {

            // Read the DataFrame from the input queue
            DataFrame* df = inputQueue->pop();

            // Sort the DataFrame
            df->sortByColumn(columnName, ascending);

            // Write the DataFrame to the output queue
            pushToOutputQueues(df);
        }
    }
};    

/**
 * @brief Class for merging and summing data in two DataFrames.
 * 
 * This class is a subclass of DataHandler.
 * It merges two DataFrames based on a column name and sums the values in the columns.
 */
class MergeAndSumHandler : public DataHandler {
public:
    /**
     * @brief Construct a new MergeAndSumHandler object.
     * 
     * @param inputQueue Reference to the input queue.
     * @param outputQueue Reference to the output queue.
     * @param ColunmName The name of the column to merge.
     * @param sumColumn The name of the column to sum.
     * @param df1 The first DataFrame to merge.
     * @param df2 The second DataFrame to merge.
     */
    MergeAndSumHandler(Queue<DataFrame*> *inputQueue, std::vector<Queue<DataFrame*>*> outputQueues)
        : DataHandler(inputQueue, outputQueues) {};

    void mergeAndSum(DataFrame& df1, DataFrame& df2, std::string columnName, std::string sumColumn) {
        while (!inputQueue->isEmpty()) {
            // Read the DataFrame from the input queue
            DataFrame* df = inputQueue->pop();

            // Merge and sum the DataFrames
            DataFrame* dfMerged = new DataFrame();
            *dfMerged = DataFrame::mergeAndSum(df1, df2, columnName, sumColumn);

            // Delete the DataFrame
            delete df;

            // Write the DataFrame to the output queue
            pushToOutputQueues(dfMerged);
        }
    }
};

#endif // DATAHANDLER_HPP
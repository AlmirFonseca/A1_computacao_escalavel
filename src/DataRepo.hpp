#ifndef DATA_REPO_HPP
#define DATA_REPO_HPP

#include <iostream>
#include "DataFrame.hpp"
#include <fstream>
#include <vector>

using namespace std;

/**
 * @brief A class representing a data repository strategy.
 * 
 * The DataRepoStrategy class provides a way to extract and load data from different sources.
 * The source is specified using "strategy" design pattern.
 * This class is an abstract class that defines the interface for the extraction and loading strategies.
*/
class DataRepoStrategy {
public:
    /**
     * @brief Extracts data from the source.
     *
     * This method extracts data from the source using the specified extraction strategy.
     * 
     * @param sourceName The source from which to extract data.
     * @param df The DataFrame object to store the extracted data.
     */
    virtual DataFrame* extractData(const string sourceName) = 0;
    
    /**
     * @brief Loads data into the source.
     *
     * This method loads data into the source using the specified loading strategy.
     * 
     * @param destName The destination to which to load data.
     * @param df The DataFrame object containing the data to be loaded.
     */
    virtual void loadData(const string destName, DataFrame& df) = 0;
};

/**
 * @brief A class representing a data repository strategy for extracting data from a csv file.
 * 
 * The CsvExtractionStrategy class provides a way to extract data from a csv file.
 * It implements the DataRepoStrategy interface.
*/
class CsvExtractionStrategy : public DataRepoStrategy {
public:
    /**
     * @brief Extracts data from the source.
     *
     * This method extracts data from the source using the csv extraction strategy.
     * 
     * @param sourceName The source from which to extract data.
     */
    DataFrame* extractData(const string sourceName) override {
        cout << "Extracting data from " << sourceName << " using csv extraction strategy." << endl;
        
        ifstream file(sourceName);
        if (file.is_open()) {
            string line;
            getline(file, line);

            // Obtain data header from the first line of the csv file
            initializer_list<string> header = {line};

            // Create a DataFrame object with the extracted header
            DataFrame* df = new DataFrame(header);
    
            while (getline(file, line)) {
                // Assuming the DataFrame has a method to add a row of data
                df->addRow(line);
            }
            file.close();

            return df;
        }

        return nullptr;
    }

    /**
     * @brief Loads data into the source.
     *
     * This method loads data into the source using the csv loading strategy.
     * 
     * @param destName The destination to which to load data.
     */
    void loadData(const string destName, DataFrame& df) override {
        cout << "Loading data into " << destName << " using csv loading strategy." << endl;

        // Set the file as output of print
        ofstream file(destName);

        // Assuming the DataFrame has a method to print its content
        // df.print(file);

        file.close();
    }
};


/**
 * @brief The DataRepo class represents a data repository that can extract and load data using different strategies.
 * 
 * The DataRepo class allows setting extraction and loading strategies for the data repository. It provides methods to extract data from a source
 * and load data into a destination using the specified strategies. It also provides a method to print the information about the data repository.
 */
class DataRepo {
private:
    string extractStrategy; /**< The extraction strategy for the data repository. */
    string loadStrategy; /**< The loading strategy for the data repository. */

public:
    /**
     * @brief Sets the extraction strategy for the data repository.
     * 
     * This method sets the extraction strategy for the data repository. The extraction strategy determines how data is extracted from a source.
     * 
     * @param sourceType The type of the data source.
     */
    void setExtractionStrategy(const string sourceType) {
        this->extractStrategy = sourceType;
        cout << "Extraction strategy set to: " << sourceType << endl;
    }

    /**
     * @brief Sets the loading strategy for the data repository.
     * 
     * This method sets the loading strategy for the data repository. The loading strategy determines how data is loaded into a destination.
     * 
     * @param sourceType The type of the data source.
     */
    void setLoadStrategy(const string sourceType) {
        this->loadStrategy = sourceType;
        cout << "Loading strategy set to: " << sourceType << endl;
    }

    /**
     * @brief Extracts data from the source using the specified extraction strategy.
     * 
     * This method extracts data from the source using the specified extraction strategy. It returns a DataFrame object containing the extracted data.
     * 
     * @param sourceName The name of the source from which to extract data.
     * @return A pointer to the DataFrame object containing the extracted data.
     */
    DataFrame* extractData(const string& sourceName) {
        if (extractStrategy == "csv") {
            CsvExtractionStrategy csvExtractionStrategy;
            return csvExtractionStrategy.extractData(sourceName);
        } else {
            cout << "Extraction strategy not supported." << endl;
            return nullptr;
        }
    }

    /**
     * @brief Loads data into the destination using the specified loading strategy.
     * 
     * This method loads data into the destination using the specified loading strategy.
     * 
     * @param destName The name of the destination to which to load data.
     * @param df The DataFrame object containing the data to be loaded.
     */
    void loadData(const string& destName, DataFrame& df) {
        if (loadStrategy == "csv") {
            CsvExtractionStrategy csvExtractionStrategy;
            csvExtractionStrategy.loadData(destName, df);
        } else {
            cout << "Loading strategy not supported." << endl;
        }
    }

    /**
     * @brief Prints the information about the data repository.
     * 
     * This method prints the extraction and loading strategies of the data repository to the console.
     */
    void printInfo() {
        cout << "Data repository extraction strategy: " << extractStrategy << endl;
        cout << "Data repository loading strategy: " << loadStrategy << endl;
    }
};

#endif
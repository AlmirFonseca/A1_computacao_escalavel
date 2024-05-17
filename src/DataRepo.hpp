#ifndef DATA_REPO_HPP
#define DATA_REPO_HPP

#include <iostream>
#include "DataFrame.hpp"
#include "Observer.hpp"
#include <fstream>
#include <sstream>
#include <vector>
#include <any>
#include <typeinfo>


using namespace std;

/**
 * @brief A class representing a data repository strategy.
 * 
 * The DataRepoStrategy class provides a way to extract and load data from different sources.
 * The source is specified using "strategy" design pattern.
 * This class is an abstract class that defines the interface for the extraction and loading strategies.
*/
class DataRepoStrategy {
private:
    /**
     * @brief Check if a string has all numeric characters.
     * 
     * This method checks if a string has all numeric characters.
     * 
     * @param str The input string.
     * @return true if the string has all numeric characters, false otherwise.
     */
    bool isNumeric(const std::string& str) {
        return !str.empty() && std::find_if(str.begin(),
                                            str.end(), [](unsigned char c) { return !std::isdigit(c); }) == str.end();
    }

    /**
     * @brief Check if a string is a float.
     * 
     * This method checks if a string is a float.
     * 
     * @param str The input string.
     * @return true if the string is a float, false otherwise.
     */
    bool isFloat(const std::string& str) {
        return !str.empty() && std::count(str.begin(), str.end(), '.') == 1 &&
            std::all_of(str.begin(), str.end(), [](unsigned char c) { return std::isdigit(c) || c == '.'; });
    }

    /**
     * @brief Infer the data type based on the input string.
     * 
     * This method infers the data type based on the input string.
     * The method checks if the string is a int, long long, float, char or string
     * 
     * @param input The input string.
     * @return The inferred data type.
     */
    const std::type_info& getDataType(string input) {
        const std::type_info& dataType = typeid(void);

        // Check if the string is a number
        if (isNumeric(input)) {
            // Check if is a long or a commom int
            try {
                stoi(input);
                return typeid(int);
            } catch (const std::out_of_range& e) {
                return typeid(long long);
            }
        } else if (isFloat(input)) {
            return typeid(float);
        } else {
            // Check if the string is either a char or a string
            if (input.length() == 1) return typeid(char);
            else return typeid(string);                
        }
    }
    
public:
    /**
     * @brief Extracts data from the source.
     *
     * This method extracts data from the source using the specified extraction strategy.
     * 
     * @param sourceName The source from which to extract data.
     * @param df The DataFrame object to store the extracted data.
     */
    virtual DataFrame* extractData(const string sourceName, const char delimiter, int startLine, vector<string> listData) = 0;
    
    /**
     * @brief Loads data into the source.
     *
     * This method loads data into the source using the specified loading strategy.
     * 
     * @param destName The destination to which to load data.
     * @param df The DataFrame object containing the data to be loaded.
     */
    virtual void loadData(DataFrame* df, string destName) = 0;

    /**
     * @brief Creates a DataFrame object with the header.
     * 
     * This method creates a DataFrame object with the header extracted from a string with column names.
    */
    DataFrame* createDataFrame(string header, char delimiter) {
        // Vector to store the column names
        vector<string> columnNames;

        // Create a stringstream from line
        stringstream ss(header);

        // Read each column data into the array
        while (ss.good()) {
            string col;
            getline(ss, col, delimiter);
            columnNames.push_back(col);
        }

        // Create a DataFrame object with the extracted header
        DataFrame* df = new DataFrame(columnNames);

        return df;
    }

    /**
     * @brief Adds a line of data to the DataFrame object.
     * 
     * This method adds a line of data to the DataFrame object by splitting the line into columns and adding the values to the DataFrame.
    */
    void addLineToDataFrame(string line, char delimiter, DataFrame& df, int& emptyCount) {
        // Split the line into a array of strings
        vector<string> data;

        // Create a stringstream from line
        stringstream ss(line);
        int numColumns = df.getColumnCount();

        // Read each column data into the array
        for (int i = 0; i < numColumns; i++) {
            string col_value;
            getline(ss, col_value, delimiter);
            
            if (col_value == "") emptyCount++;
            
            // Infer the data type of the column if the data type is not known
            const type_info& colType = (df.getColumnType(i) == typeid(void)) ? getDataType(col_value) : df.getColumnType(i);

            // Cast the value according to the data type and add it to the DataFrame
            if (colType == typeid(int)) df.addColumnValue(i, stoi(col_value));
            else if (colType == typeid(long long)) df.addColumnValue(i, stoll(col_value));
            else if (colType == typeid(float)) df.addColumnValue(i, stof(col_value));
            else if (colType == typeid(char)) df.addColumnValue(i, col_value[0]);
            else if (colType == typeid(string)) df.addColumnValue(i, convertToString(col_value));
            else df.addColumnValue(i, col_value);
    
        }

        // Increase the dataframe row count
        df.increaseRowCount();
    }
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
    DataFrame* extractData(const string sourceName, const char delimiter, int startLine, vector<string> listData = {}) override {
        cout << "Extracting data from " << sourceName << " using csv extraction strategy." << endl;
        
        ifstream file(sourceName);
        if (file.is_open()) {
            string line;
            getline(file, line);

            // Create a DataFrame object with the header
            DataFrame* df = createDataFrame(line, delimiter);
            
            // Move to the start line
            for (int i = 1; i < startLine; ++i){
                if (!getline(file, line)) {
                    printf("Start line is beyond EOF\n");
                    return nullptr;
                }
            }

            // Read the rest of the lines
            while (getline(file, line)) {
                // Count the number of empty columns in the line
                int emptyCount = 0;

                addLineToDataFrame(line, delimiter, *df, emptyCount);

                // If the number of empty columns is greater than 0, print a warning
                if (emptyCount > 0) {
                    printf("Warning: %d empty columns found in the last row (%s). Removing the row.\n\n", emptyCount, line.c_str());
                    
                    // Remove the row if there are empty columns
                    df->dropRow(df->getRowCount() - 1);
                    
                    break;
                }
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
     * @param df The DataFrame object containing the data to be loaded.
     * @param destName The destination to which to load data.
     */
    void loadData(DataFrame* df, string destName) override {
        // Set a default name for the destination if it is not provided
        if (destName == "") {
            destName = "output.csv";
        }
        
        cout << "Loading data into " << destName << " using csv loading strategy." << endl;

        // Open the file
        ofstream out(destName);

        // Write the header to the file
        for (int i = 0; i < df->getColumnCount(); i++) {
            out << df->getColumnName(i);

            // Write a comma if it is not the last column
            if (i < df->getColumnCount() - 1) {
                out << ",";
            }
        }

        out << endl;

        // Iterate over the rows of the DataFrame
        for (int i = 0; i < df->getRowCount(); i++) {
            // Iterate over the columns of the DataFrame
            for (int j = 0; j < df->getColumnCount(); j++) {
                // Write the value of the cell to the file
                out << df->getValueAt(i, j);

                // Write a comma if it is not the last column
                if (j < df->getColumnCount() - 1) {
                    out << ",";
                }
            }

            // Print a new line
            out << endl;
        }
    }
};

/**
 * @brief A class representing a data repository strategy for stracting data from a txt file.
 * 
 * The TxtExtractionStrategy class provides a way to extract data from a txt file.
 * It implements the DataRepoStrategy interface.
 */
class TxtExtractionStrategy : public DataRepoStrategy {
public:
    /**
     * @brief Extracts data from the source.
     *
     * This method extracts data from the source using the txt extraction strategy.
     * 
     * @param sourceName The source from which to extract data.
     */
    DataFrame* extractData(const string sourceName, const char delimiter, int startLine, vector<string> listData = {}) override {
        cout << "Extracting data from " << sourceName << " using txt extraction strategy." << endl;
        
        ifstream file(sourceName);
        if (file.is_open()) {
            string line;
            getline(file, line);

            // Create a DataFrame object with the header
            DataFrame* df = createDataFrame(line, delimiter);

            // Read the rest of the lines
            while (getline(file, line)) {
                // Dummy variable to count the number of columns in the line
                int emptyCount;

                addLineToDataFrame(line, delimiter, *df, emptyCount);
            }

            file.close();

            return df;
        }

        return nullptr;
    }

    /**
     * @brief Loads data into the source.
     *
     * This method loads data into the source using the txt loading strategy.
     * 
     * @param destName The destination to which to load data.
     */
    void loadData(DataFrame* df, string destName) override {
        // Set a default name for the destination if it is not provided
        if (destName == "") {
            destName = "output.txt";
        }

        cout << "Loading data into " << destName << " using txt loading strategy." << endl;

        ofstream out(destName);

        // Save cout buffer
        streambuf* coutbuf = cout.rdbuf();
        
        // Set the file as output of print
        cout.rdbuf(out.rdbuf());

        // Print the DataFrame
        df->print();

        // Restore cout buffer
        cout.rdbuf(coutbuf);

        out.close();
    }
};

/**
 * @brief A class representing a data repository strategy for extracting data from a list of strings.
 * 
 * The ListExtractionStrategy class provides a way to extract data from a list of strings.
 * It implements the DataRepoStrategy interface.
 */
class ListExtractionStrategy : public DataRepoStrategy {
public:
    /**
     * @brief Extracts data from the source.
     *
     * This method extracts data from the source using the list extraction strategy.
     * 
     * @param listData The list of strings from which to extract data.
     */
    DataFrame* extractData(const string sourceName, const char delimiter, int startLine, vector<string> listData) override {
        // Create a DataFrame object with the header
        DataFrame* df = createDataFrame(listData[0], delimiter);

        // Read the rest of the lines
        for (int i = 1; i < listData.size(); i++) {
            // Dummy variable to count the number of columns in the line
            int emptyCount;

            addLineToDataFrame(listData[i], delimiter, *df, emptyCount);
        }

        return df;
    }

    // Not implemented
    void loadData(DataFrame* df, string destName) override {
        cout << "Does not support loading data from list." << endl;
    }
};




/**
 * @brief The DataRepo class represents a data repository that can extract and load data using different strategies.
 * 
 * The DataRepo class allows setting extraction and loading strategies for the data repository. It provides methods to extract data from a source
 * and load data into a destination using the specified strategies. It also provides a method to print the information about the data repository.
 */
class DataRepo : public Observer {
private:
    string extractStrategy; /**< The extraction strategy for the data repository. */
    string loadStrategy; /**< The loading strategy for the data repository. */
    DataFrame** extractDf; /**< The DataFrame object containing the extracted data. */
    string loadFileName; /**< The name of the file to load the data into. */

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
    }

    /**
     * @brief Extracts data from the source using the specified extraction strategy.
     * 
     * This method extracts data from the source using the specified extraction strategy. It returns a DataFrame object containing the extracted data.
     * 
     * @param sourceName The name of the source from which to extract data.
     * @return A pointer to the DataFrame object containing the extracted data.
     */
    DataFrame* extractData(const string& sourceName = "", const char delimiter = ',', int startLine = 1, vector<string> listData = {}) {
        if (extractStrategy == "csv") {
            CsvExtractionStrategy csvExtractionStrategy;
            return csvExtractionStrategy.extractData(sourceName, delimiter, startLine);
        } else if (extractStrategy == "txt") {
            TxtExtractionStrategy txtExtractionStrategy;
            return txtExtractionStrategy.extractData(sourceName, delimiter, startLine);
        } else if (extractStrategy == "list") {
            ListExtractionStrategy listExtractionStrategy;
            return listExtractionStrategy.extractData("", delimiter, startLine, listData);
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
    void loadData(DataFrame* df, const string& destName="") {
        if (loadStrategy == "csv") {
            CsvExtractionStrategy csvExtractionStrategy;
            csvExtractionStrategy.loadData(df, destName);
        } else if (loadStrategy == "txt") {
            TxtExtractionStrategy txtExtractionStrategy;
            txtExtractionStrategy.loadData(df, destName);
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

    void setExtractDf(DataFrame** df) {
        this->extractDf = df;
    }

    void setLoadFileName(string fileName) {
        this->loadFileName = fileName;
    }

    // Interface for notification (update) from triggers
    void updateOnTimeTrigger() override {
        if (*extractDf == nullptr) {
            cout << "No data to load." << endl;
            return;
        }
        loadData(*extractDf, loadFileName);
        
        // Delete the old DataFrame
        delete (*extractDf);

        // Reset the pointer
        (*extractDf) = nullptr;
    }

    // Interface for handling request from triggers
    void updateOnRequestTrigger() override {
        
    }
};

#endif
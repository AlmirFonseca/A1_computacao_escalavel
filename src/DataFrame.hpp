#ifndef DATAFRAME_HPP
#define DATAFRAME_HPP

#include <algorithm>
#include <map>
#include <vector>
#include <string>
#include <any>
#include <iostream>
#include <stdexcept>
#include <iomanip>
#include <initializer_list>
#include <chrono>

#include "Series.hpp"

using namespace std;

/**
 * @brief A comparison operation enum class.
 * 
 * The CompareOperation enum class represents the different comparison operations that can be performed.
 */
enum class CompareOperation {
    EQUAL,
    NOT_EQUAL,
    GREATER_THAN,
    GREATER_THAN_OR_EQUAL,
    LESS_THAN,
    LESS_THAN_OR_EQUAL
};

/**
 * @brief Perform a comparison operation on two values of the same type.
 * 
 * This function performs a comparison operation on two values of the same type.
 * 
 * @tparam T The type of the values to be compared.
 * @param val1 The first value to be compared.
 * @param val2 The second value to be compared.
 * @param op The comparison operation to be performed.
 * @return True if the comparison is successful, false otherwise.
 */
template<typename T>
bool performComparison(T val1, T val2, CompareOperation op) {
    switch (op) {
        case CompareOperation::EQUAL:
            return val1 == val2;
        case CompareOperation::NOT_EQUAL:
            return val1 != val2;
        case CompareOperation::GREATER_THAN:
            return val1 > val2;
        case CompareOperation::GREATER_THAN_OR_EQUAL:
            return val1 >= val2;
        case CompareOperation::LESS_THAN:
            return val1 < val2;
        case CompareOperation::LESS_THAN_OR_EQUAL:
            return val1 <= val2;
        default:
            cerr << "Unsupported comparison operation." << endl;
            return false;  // Handle unknown operation
    }
}

/**
 * @brief Compare two values of any type.
 * 
 * This function compares two values of any type using the specified comparison operation.
 * 
 * @param typeInfo The type information of the values.
 * @param a The first value to be compared.
 * @param b The second value to be compared.
 * @param op The comparison operation to be performed.
 * @return True if the comparison is successful, false otherwise.
 */
bool compareValues(const type_info& typeInfo, const any& a, const any& b, CompareOperation op) {
    // Treat if the type is a string or const char* (due to the fragile string casting from any)
    if (typeInfo == typeid(string)) {
        try {
            // Cast to string and perform the comparison
            return performComparison(any_cast<string>(a), any_cast<string>(b), op);
        } catch (const bad_any_cast& e) {
            cerr << "Failed to cast string: " << e.what() << endl;
        }
    } else if (typeInfo == typeid(const char*)) {
        try {
            // Cast to string (const char*) and perform the comparison
            return performComparison(string(any_cast<const char*>(a)), string(any_cast<const char*>(b)), op);
        } catch (const bad_any_cast& e) {
            cerr << "Failed to cast const char*: " << e.what() << endl;
        }
    } 

    try {
        // Using common_type to deduce the best type for comparison
        using CommonType = common_type_t<decltype(any_cast<int>(a)), decltype(any_cast<int>(b))>;

        // Perform the cast and comparison using the common type
        CommonType val1 = any_cast<CommonType>(a);
        CommonType val2 = any_cast<CommonType>(b);

        // Perform the comparison based on the required operation
        return performComparison(val1, val2, op);
    } catch (const bad_any_cast& e) {
        cerr << "Failed to cast types for comparison: " << e.what() << endl;
    } catch (...) {
        cerr << "An error occurred during comparison." << endl;
    }

    // Return false any return point was reached
    return false;
}

/**
 * @brief A class representing a DataFrame.
 * 
 * The DataFrame class provides a way to store and manipulate tabular data.
 * It consists of a collection of columns, where each column is represented by an ISeries object.
 * The DataFrame supports adding rows of data and provides methods to retrieve information about the data.
 */
class DataFrame {
private:
    map<string, shared_ptr<ISeries>> columns; /**< A map storing the columns of the DataFrame. */
    vector<string> columnNames; /**< A vector storing the names of the columns. */
    size_t rowCount = 0; /**< The number of rows in the DataFrame. */
    long long timestamp = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count(); /**< The timestamp of the DataFrame creation. */

public:
    
    /**
     * @brief Represents a DataFrame object.
     * 
     * The DataFrame class is used to store and manipulate tabular data.
     * It provides various methods for data manipulation and analysis.
     */
    DataFrame() = default;

    
    /**
     * @brief Constructs a DataFrame object with the specified column names.
     *
     * @param names An initializer list of strings representing the column names.
     */
    DataFrame(initializer_list<string> names) {
        for (const auto& name : names) {
            // Optionally initialize each column to a new Series of a default type
            // e.g., Series<int> if you want to specify a default type
            columns[name] = make_shared<Series<int>>(name);
            columnNames.push_back(name);
        }

        // Set the timestamp to the current system time
        setCurrentTimestamp();
    }

    
    /**
     * @brief Constructs a DataFrame object with the given column names.
     * 
     * @param names A vector of strings representing the column names.
     *              Each string corresponds to a column in the DataFrame.
     */
    DataFrame(const vector<string>& names) {
        for (const auto& name : names) {
            // Optionally initialize each column to a new Series of a default type
            // e.g., Series<int> if you want to specify a default type
            columns[name] = make_shared<Series<int>>(name);
            columnNames.push_back(name);
        }

        // Set the timestamp to the current system time
        setCurrentTimestamp();
    }

    /**
     * @brief Copy constructor
     * 
     * @param other The DataFrame object to be copied.
     */
    DataFrame(const DataFrame& other) = default;

    /**
     * @brief Move constructor
     * 
     * @param other The DataFrame object to be moved.
     */
    DataFrame(DataFrame&& other) noexcept = default;

    /**
     * @brief Destructor
     */
    virtual ~DataFrame() = default;

    /**
     * @brief Copy assignment operator
     * 
     * @param other The DataFrame object to be copied.
     * @return A reference to the copied DataFrame object.
     */
    DataFrame& operator=(const DataFrame& other) = default;

    /**
     * @brief Move assignment operator
     * 
     * @param other The DataFrame object to be moved.
     * @return A reference to the moved DataFrame object.
     */
    DataFrame& operator=(DataFrame&& other) noexcept = default;

    /**
     * @brief Add a row to the DataFrame.
     * 
     * This method adds a row of data to the DataFrame. The number of values in the row must match the number of columns.
     * 
     * @tparam Args The types of the values in the row.
     * @param args The values to be added to the row.
     * @throws runtime_error If the number of values does not match the number of columns.
     */
    template<typename... Args>
    void addRow(Args... args) {
        // Check if the number of values matches the number of columns
        if (sizeof...(args) != columnNames.size()) {
            throw runtime_error("Number of values does not match number of columns.");
        }

        // Call the recursive function to add each value to the appropriate column
        addRowImpl(0, args...);
        rowCount++;
    }

    /**
     * @brief Increase the row count of the DataFrame.
     * 
     * This method increases the row count of the DataFrame by one.
     */
    void increaseRowCount() {
        rowCount++;
    }

    /**
     * @brief Get the number of rows in the DataFrame.
     * 
     * @return The number of rows in the DataFrame.
     */
    size_t getRowCount() const {
        return rowCount;
    }

    /**
     * @brief Get the number of columns in the DataFrame.
     * 
     * @return The number of columns in the DataFrame.
     */
    size_t getColumnCount() const {
        return columnNames.size();
    }

    /**
     * @brief Drop a column from the DataFrame.
     * 
     * This method removes a column from the DataFrame based on the column name.
     *  
     * @param columnName The name of the column to be removed.
     * @throws runtime_error If the column does not exist.
     */
    void dropColumn(const string& columnName) {
        auto colIt = columns.find(columnName);
        if (colIt == columns.end()) {
            throw runtime_error("Column not found." + columnName);
        }

        // Remove the column from the map
        columns.erase(colIt);

        // Remove the column name from the vector, deleting the column object
        columnNames.erase(remove(columnNames.begin(), columnNames.end(), columnName), columnNames.end());
    }

    /**
     * @brief Adds a new column to the DataFrame.
     * 
     * Adds a new column to the DataFrame with the specified column name and default value.
     * 
     * @tparam T The type of the column values.
     * @param columnName The name of the new column.
     * @param defaultValue The default value to be filled in the new column for each existing row.
     * @throws runtime_error if a column with the same name already exists.
     */
    template<typename T>
    void addColumn(const string& columnName, const T& defaultValue) {
        if (columns.find(columnName) != columns.end()) {
            throw runtime_error("Column already exists." + columnName);
        }

        // Create a new Series object for the new column
        auto newColumn = make_shared<Series<T>>(columnName);

        // Fill the column with the default value for each existing row
        for (size_t i = 0; i < rowCount; ++i) {
            newColumn->add(defaultValue);
        }

        // Add the new column to the DataFrame
        columns[columnName] = newColumn;
        columnNames.push_back(columnName);
    }

    /**
     * @brief Drop a row from the DataFrame.
     * 
     * This method removes a row from the DataFrame based on the row index.
     * 
     * @param rowIndex The index of the row to be removed.
     * @throws out_of_range If the row index is out of range.
     */
    void dropRow(size_t rowIndex) {
        // Check if the rowIndex is valid
        if (rowIndex >= rowCount) {
            throw out_of_range("Row index out of range.");
        }

        // Iterate through each column and remove the element at rowIndex
        for (auto& colPair : columns) {
            auto& series = colPair.second;
            series->removeAtIndex(rowIndex);
        }

        // Decrement the rowCount
        --rowCount;
    }


    /**
     * Sets the timestamp of the current object to the current system time.
     * The timestamp is represented as the number of milliseconds since the epoch.
     */
    void setCurrentTimestamp() {
        timestamp = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
    }

    /**
     * Sets the timestamp of the current object to the specified value.
     * The timestamp is represented as the number of milliseconds since the epoch.
     * 
     * @param ts The timestamp value to be set.
     */
    void setTimestamp(long long ts) {
        timestamp = ts;
    }

    /**
     * Gets the timestamp of the DataFrame.
     * The timestamp is represented as the number of milliseconds since the epoch.
     * 
     * @return The timestamp of the DataFrame.
     */
    long long getTimestamp() {
        return timestamp;
    }

    /**
     * @brief Print the types of each column in the DataFrame.
     * 
     * This method prints the type of each column in the DataFrame to the console.
     */
    void printColumnTypes() const {
        cout << endl << "Column Types:" << endl;
        for (const auto& [name, series] : columns) {
            cout << name << ": " << series->type().name() << endl;
        }
        cout << endl;
    }

    /**
     * @brief Filter the DataFrame by a column.
     * 
     * This method filters the DataFrame by a column based on a filter value.
     * The method removes rows that do not match the filter value.
     * 
     * @param columnName The name of the column to filter by.
     * @param filterValue The value to filter by.
     * @param op The comparison operation to use for filtering.
     * @throws runtime_error If the column does not exist.
     */
    void filterByColumn(const string& columnName, const any& filterValue, CompareOperation op) {
        auto colIt = columns.find(columnName);
        if (colIt == columns.end()) {
            throw runtime_error("Column not found: " + columnName);
        }

        const auto& columnType = colIt->second->type();  // Retrieve the type_info of the column

        // Iterate from the last index to the first
        for (size_t i = rowCount; i-- > 0;) {
            const any& columnValue = colIt->second->getDataAtIndex(i);

            // Check if the column value type matches the filter value type
            if (columnValue.type() != filterValue.type()) {
                throw runtime_error("Type mismatch error: Column value type does not match filter value type.");
            }

            bool comparisonResult = compareValues(columnType, columnValue, filterValue, op);

            // Decide whether to remove the row based on the `keep` flag and comparison result
            if (!comparisonResult) {
                // Remove the row from each Series if the values do not match the desired condition
                for (auto& [name, series] : columns) {
                    series->removeAtIndex(i);
                }
                --rowCount;
            }
        }
    }

    /**
     * @brief Merge two DataFrames.
     * 
     * This method merges two DataFrames based on a common column.
     * The method assumes that the column is ordered in both DataFrames.
     * This method results in a new DataFrame with the merged data ordered by the common column.
     * 
     * @param df1 The first DataFrame to be merged.
     * @param df2 The second DataFrame to be merged.
     * @param columnName The name of the column to merge on.
     * @return The merged DataFrame.
     * @throws runtime_error If the column is not found in both DataFrames.
     * @throws runtime_error If the column types do not match.
     */
    static DataFrame mergeOrdered(const DataFrame& df1, const DataFrame& df2, const string& columnName) {;
        // Check if the column exists in both DataFrames
        if (df1.columns.find(columnName) == df1.columns.end() || df2.columns.find(columnName) == df2.columns.end()) {
            throw runtime_error("Column not found in both DataFrames.");
        }

        // Check if the column types match
        if (df1.columns.at(columnName)->type() != df2.columns.at(columnName)->type()) {
            throw runtime_error("Column types do not match.");
        }

        // Instantiate a new dataframe to store the merged result (assuming that both dataframes have the same column names)
        DataFrame merged = deepCopy(df1, false); // Copy the column names/types but not the data

        // Assuming that both dataframe has the column ordered, just merge the data
        size_t i = 0, j = 0;
        while (i < df1.rowCount && j < df2.rowCount) {
            try {
                // Cast the values to long for comparison
                long val1 = static_cast<long>(stol(df1.columns.at(columnName)->getStringAtIndex(i)));
                long val2 = static_cast<long>(stol(df2.columns.at(columnName)->getStringAtIndex(j)));
                
                if (val1 <= val2) {
                    // Clone the value of each column in the row
                    for (const auto& colName : df1.columnNames) merged.cloneValue(colName, df1, i, colName);
                    i++;
                } else {
                    // Clone the value of each column in the row
                    for (const auto& colName : df2.columnNames) merged.cloneValue(colName, df2, j, colName);
                    j++;
                }
            } catch (const bad_any_cast& e) {
                cout << e.what() << endl;
                throw runtime_error("Type mismatch error: Unable to cast value to long: ");
            }
        }

        // Append remaining rows from either dataframe
        while (i < df1.rowCount) {
            // Clone the value of each column in the row
            for (const auto& colName : df1.columnNames) merged.cloneValue(colName, df1, i, colName);
            i++;
        }

        while (j < df2.rowCount) {
            // Clone the value of each column in the row
            for (const auto& colName : df2.columnNames) merged.cloneValue(colName, df2, j, colName);
            j++;
        }

        // Update the row count
        merged.rowCount = df1.rowCount + df2.rowCount;

        return merged;
    }

    /**
     * @brief Print the DataFrame.
     * 
     * This method prints the entire DataFrame to the console.
     */
    void print() const {
        print(0, rowCount - 1);
    }

    /**
     * @brief Print a range of rows from the DataFrame.
     * 
     * This method prints a range of rows from the DataFrame to the console.
     * 
     * @param startIndex The index of the first row to be printed.
     * @param endIndex The index of the last row to be printed.
     */
    void print(size_t startIndex, size_t endIndex) const {
        if (endIndex >= rowCount) endIndex = rowCount - 1; // Ensure endIndex is within bounds

        // Check if the dataframe is empty
        if (rowCount == 0 || endIndex < startIndex) {
            cout << "DataFrame is empty." << endl;
            return;
        }

        cout << endl;
        for (size_t i = 0; i < columnNames.size(); ++i) {
            cout << "----------------";
        }
        cout << endl;

        // Print column headers
        for (const auto& columnName : columnNames) {
            cout << columnName << "\t\t";
        }

        cout << endl;
        for (size_t i = 0; i < columnNames.size(); ++i) {
            cout << "################";
        }
        cout << endl;

        // Print data for each row in the specified range
        for (size_t rowIndex = startIndex; rowIndex <= endIndex; ++rowIndex) {
            for (const auto& columnName : columnNames) {
                // Directly access and print the data for each column at rowIndex
                cout << columns.at(columnName)->getStringAtIndex(rowIndex) << "\t\t";
            }
            cout << endl;
        }
        
        for (size_t i = 0; i < columnNames.size(); ++i) {
            cout << "----------------";
        }

        // Print the timestamp
        cout << endl << "Timestamp: " << timestamp << endl;

        cout << endl;
    }

    /**
     * @brief Get a pointer to a column in the DataFrame.
     * 
     * This method returns a shared pointer to a column in the DataFrame based on the column name.
     * 
     * @param name The name of the column to be retrieved.
     * @return A shared pointer to the column.
     * @throws runtime_error If the column is not found.
     */
    shared_ptr<ISeries> getColumnPtr(const string& name) const {
        auto it = columns.find(name);
        if (it == columns.end()) throw runtime_error("Column not found");
        return it->second;
    }

    /**
     * @brief Clone the value of a column in a row.
     * 
     * This method clones the value of a column in a row from another DataFrame into this DataFrame.
     * 
     * @param srcName The name of the column in the source DataFrame.
     * @param srcDf The source DataFrame.
     * @param index The index of the row in the source DataFrame.
     * @param targetName The name of the column in this DataFrame.
     */
    void cloneValue(const string& srcName, const DataFrame& srcDf, size_t index, const string& targetName) {
        auto targetSeries = getColumnPtr(targetName);
        auto srcSeries = srcDf.getColumnPtr(srcName);
        targetSeries->addFromSeries(srcSeries.get(), index);
    }

    /**
     * @brief Deep copy the contents of another DataFrame.
     * 
     * This method deep copies the contents of another DataFrame into this DataFrame.
     * 
     * @param other The DataFrame to be copied.
     * @param copyData A flag to indicate whether to copy the data as well.
     */
    void deepCopyImpl(const DataFrame& other, bool copyData = true) {
        columnNames = other.columnNames; // Copy column names
        columns.clear(); // Clear existing columns if any

        if (copyData) rowCount = other.rowCount; // Copy the row count
        else rowCount = 0;

        for (const auto& name : columnNames) {
            columns[name] = other.columns.at(name)->clone();
            if (!copyData) columns[name]->clear();
        }

        // Copy the timestamp
        timestamp = other.timestamp;
    }

    /**
     * @brief Deep copy the contents of another DataFrame.
     * 
     * This method deep copies the contents of another DataFrame and returns the result as a new DataFrame.
     * 
     * @param other The DataFrame to be copied.
     * @param copyData A flag to indicate whether to copy the data as well.
     * @return The copied DataFrame.
     */
    static DataFrame deepCopy(const DataFrame& other, bool copyData = true) {
        DataFrame result;
        result.deepCopyImpl(other, copyData);
        return result;
    }

    /**
     * @brief Concatenate another DataFrame to this DataFrame.
     * 
     * This method concatenates another DataFrame to this DataFrame.
     * The method assumes that the column names of both DataFrames match.
     * 
     * @param other The DataFrame to be concatenated.
     * @throws runtime_error If the column names do not match.
     */
    void concat(const DataFrame& other) {
        // Check if the number of columns match
        if (columnNames != other.columnNames) {
            throw runtime_error("Column names do not match.");
        } else {
            // Check if each column names and types matches
            for (const auto& name : columnNames) {
                if (columns.at(name)->type() != other.columns.at(name)->type()) {
                    throw runtime_error("Column types do not match.");
                }
            }
        }

        // Concatenate each row of data from the other DataFrame
        for (size_t i = 0; i < other.rowCount; ++i) {
            for (const auto& name : columnNames) {
                cloneValue(name, other, i, name);
            }
        }

        // Update the row count
        rowCount += other.rowCount;
    }

    /**
     * @brief Concatenate two DataFrames.
     * 
     * This method concatenates two DataFrames and returns the result as a new DataFrame.
     * 
     * @param df1 The first DataFrame to be concatenated.
     * @param df2 The second DataFrame to be concatenated.
     * @return The concatenated DataFrame.
     */
    static DataFrame concat(const DataFrame& df1, const DataFrame& df2) {
        // Deep copy the first dataframe
        DataFrame result = deepCopy(df1);

        // Apply the concat method to the copied dataframe
        result.concat(df2);

        return result;
    }

    /**
     * @brief Recursive template function to handle each column in the row.
     * 
     * This function is used internally to add each value in the row to the appropriate column.
     * 
     * @tparam T The type of the first value in the row.
     * @tparam Args The types of the rest of the values in the row.
     * @param index The index of the current column being processed.
     * @param first The first value in the row.
     * @param rest The rest of the values in the row.
     * @throws runtime_error If the index is out of bounds.
     * @throws runtime_error If there is a type mismatch error when adding a value to a Series.
     */
    template<typename T, typename... Args>
    void addRowImpl(size_t index, T first, Args... rest) {
        if (index >= columnNames.size()) {
            throw runtime_error("Index out of bounds.");
        }

        // For the first row, create the appropriate Series instance
        if (rowCount == 0) columns[columnNames[index]] = make_shared<Series<T>>(columnNames[index]);

        // Add the value to the appropriate Series
        try {
            auto& series = columns[columnNames[index]];

            // Add the value to the Series, casting it appropriately
            try {
                series->add(first);
            } catch (const exception &e) {
                cout << "Error while casting and adding value " + convertToString(first) + " to Series: " << e.what() << endl;
                throw runtime_error("Type mismatch error: Unable to add value to Series.");
            }

        } catch (const bad_any_cast& e) {
            throw runtime_error("Type mismatch error: Unable to add value to Series.");
        }

        // Recursive call for the rest of the arguments
        if constexpr (sizeof...(rest) > 0) {
            addRowImpl(index + 1, rest...);
        }
    }

    
    /**
     * Adds a value to a specific column in the DataFrame.
     * 
     * @tparam T The type of the value being added.
     * @param index The index of the column.
     * @param first The value to be added.
     * @throws runtime_error if the index is out of bounds or if there is a type mismatch.
     */
    template<typename T>
    void addColumnValue(size_t index, T first) {
        if (index >= columnNames.size()) {
            throw runtime_error("Index out of bounds.");
        }

        // For the first row, create the appropriate Series instance
        if (rowCount == 0) columns[columnNames[index]] = make_shared<Series<T>>(columnNames[index]);

        // Add the value to the appropriate Series
        try {
            auto& series = columns[columnNames[index]];

            // Add the value to the Series, casting it appropriately
            try {
                series->add(first);
            } catch (const exception &e) {
                cout << "Error while casting and adding value " + convertToString(first) + " to Series: " << e.what() << endl;
                throw runtime_error("Type mismatch error: Unable to add value to Series.");
            }
        } catch (const bad_any_cast& e) {
            throw runtime_error("Type mismatch error: Unable to add value to Series.");
        }
    }

    /**
     * @brief Get the index of a column based on the column name.
     * 
     * This method returns the index of a column based on the column name.
     * 
     * @param columnName The name of the column.
     * @return The index of the column.
     * @throws runtime_error If the column is not found.
     */
    size_t getColumnIndex(const string& columnName) {
        for (size_t i = 0; i < columnNames.size(); i++) {
            if (columnNames[i] == columnName) {
                return i;
            }
        }
        throw runtime_error("Column not found.");
    }

    /**
     * @brief Return a value based on the column and row index.
     * 
     * This method returns a value from the DataFrame based on the column and row index.
     * 
     * @param rowIndex The index of the row.
     * @param columnIndex The index of the column.
     * @return The value at the specified row and column.
     * @throws runtime_error If the row or column index is out of bounds.
     */
    string getValueAt(size_t rowIndex, size_t columnIndex) {
        if (rowIndex >= rowCount || columnIndex >= columnNames.size()) {
            throw runtime_error("Index out of bounds.");
        }

        return columns[columnNames[columnIndex]]->getStringAtIndex(rowIndex);
    }

    /**
     * @brief Return a column name
     * 
     * This method returns the name of a column based on the column index.
     * 
     * @param columnIndex The index of the column.
     * @return The name of the column.
     * @throws runtime_error If the column index is out of bounds.
     */
    string getColumnName(size_t columnIndex) {
        if (columnIndex >= columnNames.size()) {
            throw runtime_error("Index out of bounds.");
        }

        return columnNames[columnIndex];
    }


    /**
     * @brief Calculate the sum of a column in the DataFrame.
     * 
     * This method calculates the sum of a column in the DataFrame based on the column index.
     * 
     * @param columnIndex The index of the column to calculate the sum for.
     * @return The sum of the column.
     * @throws runtime_error If the column does not exist.
     */
    any sum(size_t columnIndex) {
        if (columnIndex >= columnNames.size()) {
            throw runtime_error("Column not found.");
        }

        return columns[columnNames[columnIndex]]->sum();
    }

    /**
     * @brief Calculate the sum of a column in the DataFrame.
     * 
     * This method calculates the sum of a column in the DataFrame based on the column name.
     * 
     * @param columnName The name of the column to calculate the sum for.
     * @return The sum of the column.
     * @throws runtime_error If the column does not exist.
     */
    any sum(const string& columnName) {
        return sum(getColumnIndex(columnName));
    }

    /**
     * @brief Count the occurrences of each value in a column.
     * 
     * This method counts the occurrences of each value in a column and returns the result as a new DataFrame.
     * 
     * @param columnIndex The index of the column to count the occurrences for.
     * @return A DataFrame containing the value counts.
     * @throws runtime_error If the column index is out of bounds.
     */
    DataFrame valueCounts(size_t columnIndex) {
        if (columnIndex >= columnNames.size()) {
            throw runtime_error("Column index out of bounds.");
        }

        // Get the column name using the index
        string columnName = columnNames[columnIndex];
        auto columnSeries = columns[columnName];

        // Map to store value counts
        map<string, int> valueCountMap;

        // Iterate through the column and count the occurrences of each value
        for (size_t i = 0; i < rowCount; ++i) {
            string value = columnSeries->getStringAtIndex(i);
            if (valueCountMap.find(value) == valueCountMap.end()) {
                valueCountMap[value] = 1;
            } else {
                valueCountMap[value]++;
            }
        }

        // Create a new DataFrame to store the value counts
        DataFrame countDataFrame({"Value", "Count"});
        for (const auto& [value, count] : valueCountMap) {
            countDataFrame.addRow(value, count);
        }

        return countDataFrame;
    }

    /**
     * @brief Count the occurrences of each value in a column.
     * 
     * This method counts the occurrences of each value in a column and returns the result as a new DataFrame.
     * 
     * @param columnName The name of the column to count the occurrences for.
     * @return A DataFrame containing the value counts.
     * @throws runtime_error If the column does not exist.
     */
    DataFrame valueCounts(const string& columnName) {
        return valueCounts(getColumnIndex(columnName));
    }
    /**
     * @brief Sort the DataFrame by a column.
     *
     * This method sorts the DataFrame by a column in ascending order.
     * The method assumes that the column exists in the DataFrame.
     * 
     * @param columnIndex The index of the column to sort by.
     * @param ascending The order of sorting (ascending or descending).
     * @throws runtime_error If the column index is out of bounds.
     */
    void sortByColumn(size_t columnIndex, bool ascending = true) {
        // Get the column object and type
        auto column = columns[getColumnName(columnIndex)];
        const auto& columnType = column->type();

        // Create a vector of pairs to store the index and value of each row
        vector<pair<size_t, any>> values;

        // Iterate through the column and store the index and value of each row
        for (size_t i = 0; i < rowCount; ++i) {
            values.push_back({i, column->getDataAtIndex(i)});
        }

        // Sort the vector of pairs based on the value of the column
        sort(values.begin(), values.end(), [&](const auto& a, const auto& b) {
            if (ascending) {
                return compareValues(columnType, a.second, b.second, CompareOperation::LESS_THAN);
            } else {
                return compareValues(columnType, a.second, b.second, CompareOperation::GREATER_THAN);
            }
        });

        // Create a new DataFrame to store the sorted data
        DataFrame sortedDf = deepCopy(*this, false);

        // Add the rows to the new DataFrame in the sorted order
        for (const auto& [index, value] : values) {
            for (const auto& [name, series] : columns) {
                sortedDf.columns[name]->addFromSeries(series.get(), index);
            }
        }

        // Update the row count
        sortedDf.rowCount = rowCount;

        // Copy the sorted DataFrame back to the original DataFrame
        deepCopyImpl(sortedDf);
    }

    /**
     * @brief Sort the DataFrame by a column.
     *
     * This method sorts the DataFrame by a column in ascending order.
     * The method assumes that the column exists in the DataFrame.
     * 
     * @param columnName The name of the column to sort by.
     * @param ascending The order of sorting (ascending or descending).
     * @throws runtime_error If the column does not exist.
     */
    void sortByColumn(const string& columnName, bool ascending = true) {
        sortByColumn(getColumnIndex(columnName), ascending);
    }

    /**
     * @brief Left join this DataFrame with another DataFrame on a given key column.
     * 
     * @param right The right DataFrame to join with.
     * @param keyColumnName The name of the column used as a key for joining.
     * @return A new DataFrame resulting from the left join operation.
     * @throws runtime_error If the key column is not found in either DataFrame.
     */
    DataFrame leftJoin(const DataFrame& right, const string& keyColumnName, const bool dropKeyColumn = false) {
        // Check if the column exists in both DataFrames
        if (columns.find(keyColumnName) == columns.end() || right.columns.find(keyColumnName) == right.columns.end()) {
            throw runtime_error("Column not found in both DataFrames.");
        }

        // Create a new DataFrame to store the result
        DataFrame result = deepCopy(*this, true);

        // Get the key column of each dataframe
        shared_ptr<ISeries> leftKeyColumn = columns[keyColumnName];
        shared_ptr<ISeries> rightKeyColumn = right.columns.at(keyColumnName);

        // Create a map to store the index of each key in the right DataFrame
        map<string, size_t> keyIndexMap;
        for (size_t i = 0; i < right.rowCount; ++i) {
            keyIndexMap[rightKeyColumn->getStringAtIndex(i)] = i;
        }

        // Add the columns of the right DataFrame to the result
        for (const auto& [name, series] : right.columns) {
            // Skip the key column if it is the same as the key column in this DataFrame
            if (name != keyColumnName) {
                // Clone, clear the data, and add the column name
                result.columns[name] = series->clone();
                result.columns[name]->clear();
                result.columnNames.push_back(name);
            }
        }

        // Add the rows from the right DataFrame to the result based on the key column
        for (size_t i = 0; i < rowCount; ++i) {
            string key = leftKeyColumn->getStringAtIndex(i);

            // Iterate through each column in the right DataFrame
            for (const auto& [name, series] : right.columns) {
                // Skip the key column
                if (name == keyColumnName) continue;
                
                // If the key exists in the right DataFrame, add the row
                if (keyIndexMap.find(key) != keyIndexMap.end()) {
                    result.columns[name]->addFromSeries(series.get(), keyIndexMap[key]);
                } 
                else { // Otherwise, add null values for the columns from the right DataFrame
                    result.columns[name]->addNull();
                }
            }
        }

        // Drop the key column if specified
        if (dropKeyColumn) {
            result.dropColumn(keyColumnName);
        }

        return result;
    }

    /**
     * @brief Return the type of a column based on the column index.
     *
     * This method returns the type of a column based on the column index.
     *
     * @param columnIndex The index of the column.
     * @return The type of the column.
     * @throws runtime_error If the column index is out of bounds.
     */
    const type_info& getColumnType(int columnIndex) {

        // Check if the column index is out of bounds
        if (columnIndex >= columnNames.size()) {
            throw runtime_error("Index out of bounds.");
        }

        // Check if there is any row in the dataframe
        if (rowCount == 0) return typeid(void);
        else return columns[columnNames[columnIndex]]->type();
    }

    /**
     * @brief Merge two DataFrames on an ID column and sum another column.
     *
     * This method merges two DataFrames based on an ID column and sums up the values of another column.
     *
     * @param df1 The first DataFrame.
     * @param df2 The second DataFrame.
     * @param idColumnName The name of the ID column to merge on.
     * @param sumColumnName The name of the column to sum.
     * @return A new DataFrame with ID and sum of the specified column.
     * @throws runtime_error If the ID or sum column is not found or if there are type mismatches.
     */
    static DataFrame mergeAndSum(DataFrame& df1, DataFrame& df2, const string& idColumnName, const string& sumColumnName) {
        // If there is no column in ID column, return a dataframe with the sum of the count column
        if (idColumnName == "") {
            int sum = any_cast<int>(df1.sum(sumColumnName)) + any_cast<int>(df2.sum(sumColumnName));
            DataFrame* result = new DataFrame({sumColumnName});
            result->addRow(sum);
            // Add the dataframe timestamp
            result->setTimestamp(df1.getTimestamp());
            return *result;
        }

        // Ensure both DataFrames have the required columns
        if (df1.columns.find(idColumnName) == df1.columns.end() || df2.columns.find(idColumnName) == df2.columns.end() ||
            df1.columns.find(sumColumnName) == df1.columns.end() || df2.columns.find(sumColumnName) == df2.columns.end()) {
            throw runtime_error("Required columns not found in one or both DataFrames.");
        }

        // Check for type compatibility for summing operations
        if (df1.columns.at(sumColumnName)->type() != typeid(int) ||
            df2.columns.at(sumColumnName)->type() != typeid(int)) {
            throw runtime_error("Sum column must be of type int or double.");
        }

        // Use a map to accumulate sums based on ID
        map<string, int> sumMap;
        for (size_t i = 0; i < df1.getRowCount(); ++i) {
            string id = df1.columns.at(idColumnName)->getStringAtIndex(i);
            int value = any_cast<int>(df1.columns.at(sumColumnName)->getDataAtIndex(i));
            sumMap[id] = sumMap[id] + value;
        }

        for (size_t i = 0; i < df2.getRowCount(); ++i) {
            string id = df2.columns.at(idColumnName)->getStringAtIndex(i);
            int value = any_cast<int>(df2.columns.at(sumColumnName)->getDataAtIndex(i));
            sumMap[id] = sumMap[id] + value;
        }

        // Create a new DataFrame to store the result
        DataFrame result({idColumnName, sumColumnName});
        for (auto& [id, sum] : sumMap) {
            result.addRow(id, sum);
        }
        // Add the dataframe timestamp
        result.setTimestamp(df1.getTimestamp());

        return result;
    }

};



#endif // DATAFRAME_HPP

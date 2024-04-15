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

#include "Series.hpp"

using namespace std;

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

public:
    // /**
    //  * @brief Constructor that takes an initializer list of column names.
    //  * 
    //  * @param names An initializer list of column names.
    //  */

    // Default constructor
    DataFrame() = default;

    // Constructor that takes an initializer list of column names
    DataFrame(initializer_list<string> names) {
        for (const auto& name : names) {
            // Optionally initialize each column to a new Series of a default type
            // e.g., Series<int> if you want to specify a default type
            columns[name] = make_shared<Series<int>>(name);
            columnNames.push_back(name);
        }
    }

    // Constructor for initializing with a vector of column names
    DataFrame(const vector<string>& names) {
        for (const auto& name : names) {
            // Optionally initialize each column to a new Series of a default type
            // e.g., Series<int> if you want to specify a default type
            columns[name] = make_shared<Series<int>>(name);
            columnNames.push_back(name);
        }
    }

    // Copy constructor
    DataFrame(const DataFrame& other) = default;

    // Move constructor
    DataFrame(DataFrame&& other) noexcept = default;

    // Destructor
    virtual ~DataFrame() = default;

    // Assignment operators
    DataFrame& operator=(const DataFrame& other) = default;
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

        // Check if the type of each value matches the type of the corresponding column
        
        // Call the recursive function to add each value to the appropriate column
        addRowImpl(0, args...);
        rowCount++;
    }

    // Add a single value to a column
    template<typename T>
    void addSingleValue(T value, int columnIndex) {
        addValueImpl(columnIndex, value);
        rowCount++;
    }

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
     * @throws std::runtime_error if a column with the same name already exists.
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
     * @throws std::out_of_range If the row index is out of range.
     */
    void dropRow(size_t rowIndex) {
        // Check if the rowIndex is valid
        if (rowIndex >= rowCount) {
            throw std::out_of_range("Row index out of range.");
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
     * @brief Print the types of each column in the DataFrame.
     * 
     * This method prints the type of each column in the DataFrame to the console.
     */
    void printColumnTypes() const {
        for (const auto& [name, series] : columns) {
            cout << name << ": " << series->type().name() << endl;
        }
    }

    /**
     * @brief Filter the DataFrame by a column.
     * 
     * This method filters the DataFrame by a column based on a filter value.
     * The method removes rows that do not match the filter value.
     * 
     * @param columnName The name of the column to filter by.
     * @param filterValue The value to filter by.
     * @param keep A flag to indicate whether to keep or remove rows that match the filter value.
     * @throws std::runtime_error If the column does not exist.
     */
    void filterByColumn(const std::string& columnName, const std::any& filterValue, bool keep = true) {
        auto colIt = columns.find(columnName);
        if (colIt == columns.end()) {
            throw std::runtime_error("Column not found: " + columnName);
        }

        const auto& columnType = colIt->second->type();  // Retrieve the type_info of the column

        // Iterate from the last index to the first
        for (size_t i = rowCount; i-- > 0;) {
            const std::any& columnValue = colIt->second->getDataAtIndex(i);
            bool comparisonResult = compareAny(columnType, columnValue, filterValue);

            // Decide whether to remove the row based on the `keep` flag and comparison result
            if ((keep && !comparisonResult) || (!keep && comparisonResult)) {
                // Remove the row from each Series if the values do not match the desired condition
                for (auto& [name, series] : columns) {
                    series->removeAtIndex(i);
                }
                --rowCount;
            }
        }
    }

    /**
     * @brief Compare two std::any values of the same type.
     * 
     * This function compares two std::any values of the same type.
     * It is used to compare the values of a column with a filter value.
     * 
     * @param typeInfo The type_info of the values to be compared.
     * @param a The first value to be compared.
     * @param b The second value to be compared.
     * @return true if the values match, false otherwise.
     */
    bool compareAny(const std::type_info& typeInfo, const std::any& a, const std::any& b) {
        if (a.type() != b.type()) return false;  // Early exit if types don't match directly

        try {
            if (typeInfo == typeid(int)) {
                return std::any_cast<int>(a) == std::any_cast<int>(b);
            } else if (typeInfo == typeid(double)) {
                return std::any_cast<double>(a) == std::any_cast<double>(b);
            } else if (typeInfo == typeid(std::string)) {
                return std::any_cast<std::string>(a) == std::any_cast<std::string>(b);
            } else if (typeInfo == typeid(char)) {
                // Special handling for chars to avoid treating them as integers
                return std::any_cast<char>(a) == std::any_cast<char>(b);
            }
            // Add more types as needed
        } catch (const std::bad_any_cast&) {
            // Log error or handle exception as needed
            return false; // Return false if any cast fails
        }

        return false; // If type is not handled, return false
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
     * @throws std::runtime_error If the column is not found in both DataFrames.
     * @throws std::runtime_error If the column types do not match.
     */
    static DataFrame mergeOrdered(const DataFrame& df1, const DataFrame& df2, const string& columnName) {
        DataFrame result;
        // Check if the column exists in both DataFrames
        if (df1.columns.find(columnName) == df1.columns.end() || df2.columns.find(columnName) == df2.columns.end()) {
            throw runtime_error("Column not found in both DataFrames.");
        }

        // Check if the column types match
        if (df1.columns.at(columnName)->type() != df2.columns.at(columnName)->type()) {
            throw runtime_error("Column types do not match.");
        }

        // Instantiate a new dataframe to store the merged result (assuming that both dataframes have the same column names)
        DataFrame* merged = new DataFrame();
        merged->deepCopy(df1, false); // Copy the column names/types but not the data

        // Assuming that both dataframe has the column ordered, just merge the data
        size_t i = 0, j = 0;
        while (i < df1.rowCount && j < df2.rowCount) {
            try {
                // Cast the values to long for comparison
                long val1 = static_cast<long>(stol(df1.columns.at(columnName)->getStringAtIndex(i)));
                long val2 = static_cast<long>(stol(df2.columns.at(columnName)->getStringAtIndex(j)));
                
                if (val1 <= val2) {
                    // Clone the value of each column in the row
                    for (const auto& colName : df1.columnNames) merged->cloneValue(colName, df1, i, colName);
                    i++;
                } else {
                    // Clone the value of each column in the row
                    for (const auto& colName : df2.columnNames) merged->cloneValue(colName, df2, j, colName);
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
            for (const auto& colName : df1.columnNames) merged->cloneValue(colName, df1, i, colName);
            i++;
        }

        while (j < df2.rowCount) {
            // Clone the value of each column in the row
            for (const auto& colName : df2.columnNames) merged->cloneValue(colName, df2, j, colName);
            j++;
        }

        // Update the row count
        merged->rowCount = df1.rowCount + df2.rowCount;

        return *merged;
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

        // Print column headers
        for (const auto& columnName : columnNames) {
            cout << columnName << "\t";
        }
        cout << endl;

        // Print data for each row in the specified range
        for (size_t rowIndex = startIndex; rowIndex <= endIndex; ++rowIndex) {
            for (const auto& columnName : columnNames) {
                // Directly access and print the data for each column at rowIndex
                cout << columns.at(columnName)->getStringAtIndex(rowIndex) << "\t";
            }
            cout << endl;
        }
    }

    /**
     * @brief Get a pointer to a column in the DataFrame.
     * 
     * This method returns a shared pointer to a column in the DataFrame based on the column name.
     * 
     * @param name The name of the column to be retrieved.
     * @return A shared pointer to the column.
     * @throws std::runtime_error If the column is not found.
     */
    std::shared_ptr<ISeries> getColumnPtr(const std::string& name) const {
        auto it = columns.find(name);
        if (it == columns.end()) throw std::runtime_error("Column not found");
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
    void cloneValue(const std::string& srcName, const DataFrame& srcDf, size_t index, const std::string& targetName) {
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
    void deepCopy(const DataFrame& other, bool copyData = true) {
        columnNames = other.columnNames; // Copy column names
        columns.clear(); // Clear existing columns if any

        if (copyData) rowCount = other.rowCount; // Copy the row count
        else rowCount = 0;

        for (const auto& name : columnNames) {
            columns[name] = other.columns.at(name)->clone();
            if (!copyData) columns[name]->clear();
        }
    }

private:
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
        if (rowCount == 0) {
            columns[columnNames[index]] = make_shared<Series<T>>(columnNames[index]);
        }

        // Add the value to the appropriate Series
        try {
            auto& series = columns[columnNames[index]];
            series->add(first);
        } catch (const bad_any_cast& e) {
            throw runtime_error("Type mismatch error: Unable to add value to Series.");
        }

        // Recursive call for the rest of the arguments
        if constexpr (sizeof...(rest) > 0) {
            addRowImpl(index + 1, rest...);
        }
    }

    // function that do the same job as addRowImpl but for one value to one column
    // remeber to create the appropriate Series instance
    template<typename T>
    void addValueImpl(size_t index, T first) {
        if (index >= columnNames.size()) {
            throw runtime_error("Index out of bounds.");
        }

        // If it's a any type, convert to string
        if (typeid(T) == typeid(std::any)) {
            columns[columnNames[index]]->add(std::any_cast<std::string>(first));
            return;
        }

        // For the first row, create the appropriate Series instance
        if (rowCount == 0) {
            columns[columnNames[index]] = make_shared<Series<T>>(columnNames[index]);
        }

        // Add the value to the appropriate Series
        try {
            auto& series = columns[columnNames[index]];
            series->add(first);
        } catch (const bad_any_cast& e) {
            throw runtime_error("Type mismatch error: Unable to add value to Series.");
        }
    }
};

#endif // DATAFRAME_HPP

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
    // DataFrame(initializer_list<string> names) : columnNames(names) {}

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
     * @brief Filter the DataFrame by a column value.
     * 
     * This method filters the DataFrame by a column value.
     * It removes rows where the value in the specified column does not match the filter value.
     * 
     * @param columnName The name of the column to filter by.
     * @param filterValue The value to filter by.
     * @throws std::runtime_error If the column does not exist.
     */
    void filterByColumn(const std::string& columnName, const std::any& filterValue) {
        auto colIt = columns.find(columnName);
        if (colIt == columns.end()) {
            throw std::runtime_error("Column not found: " + columnName);
        }

        const auto& columnType = colIt->second->type();  // Retrieve the type_info of the column
        for (size_t i = rowCount - 1; i != (size_t)-1; --i) {
            const std::any& columnValue = colIt->second->getDataAtIndex(i);

            if (!compareAny(columnType, columnValue, filterValue)) {

                // Remove the row from each Series if the values do not match
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
};

#endif // DATAFRAME_HPP

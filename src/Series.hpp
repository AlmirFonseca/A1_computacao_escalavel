#ifndef SERIES_HPP
#define SERIES_HPP

#include <any>
#include <cstddef>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <vector>
#include <numeric>
#include <unordered_set>

using namespace std;

/**
 * Converts a value to a string representation.
 * 
 * This function converts the given value to a string representation. If the value is of type string or const char*, it is directly converted to a string. Otherwise, it is converted using the to_string function.
 * 
 * @tparam T The type of the value to be converted.
 * @param value The value to be converted.
 * @return The string representation of the value.
 */
template<typename T>
string convertToString(const T& value) {
    if constexpr (is_same_v<T, string>) {
        return value; // Directly return string values
    } else if constexpr (is_same_v<T, const char*> || is_same_v<T, char*>) {
        return string(value); // Convert C-style string to string
    } else if constexpr (is_same_v<T, char>) {
        // Handle char type specifically to avoid treating it as an integer
        return string(1, value);
    } else if constexpr (is_arithmetic_v<T>) {
        return to_string(value); // Use to_string for numeric types
    } else {
        // Fallback for types not directly supported by to_string
        // This requires a way to convert custom types to string
        static_assert(is_arithmetic_v<T> || is_same_v<T, char>, "Type not supported for conversion to string.");
        return ""; // Placeholder return for unsupported types to satisfy all control paths
    }
}


// Interface for Series
/**
 * @brief Interface for a series data structure.
 * 
 * This interface defines the common operations that can be performed on a series.
 */
class ISeries {
public:

    /**
     * @brief Constructor for the ISeries class.
     */
    ISeries() = default;

    /**
     * @brief Destructor for the ISeries class.
     */
    virtual ~ISeries() {}

    /**
     * @brief Returns the type information of the series.
     * 
     * @return The type information of the series.
     */
    virtual const type_info& type() const = 0;

    /**
     * @brief Returns the size of the series.
     * 
     * @return The size of the series.
     */
    virtual size_t size() const = 0;

    /**
     * @brief Adds a value to the series.
     * 
     * @param value The value to be added.
     */
    virtual void add(const any& value) = 0;

    /**
     * @brief Adds a null value to the series, according to the type of the series.
     */
    virtual void addNull() = 0;

    /**
     * @brief Removes the data at the specified index in the series.
     * 
     * @param index The index of the data to remove.
     */
    virtual void removeAtIndex(size_t index) = 0;

    /**
     * @brief Clears the series data.
     */
    virtual void clear() = 0;

    /**
     * @brief Returns the data at the specified index in the series.
     * 
     * @param index The index of the data to be accessed.
     * @return The data at the specified index in the series.
     */
    virtual any getDataAtIndex(size_t index) const = 0;

    /**
     * @brief Returns a string representation of the data at the specified index in the series.
     * 
     * @param index The index of the data to be accessed.
     * @return The string representation of the data at the specified index in the series.
     */
    virtual string getStringAtIndex(size_t index) const = 0;

    /**
     * @brief Computes the sum of the elements in the series.
     * 
     * @return The sum of the elements in the series.
     */
    virtual any sum() const = 0;

    /**
     * @brief Computes the mean of the elements in the series.
     * 
     * @return The mean of the elements in the series.
     */
    virtual double mean() const = 0;

    /**
     * @brief Prints the series.
     */
    virtual void print() const = 0;
    
    /**
     * @brief Adds a value to the series from another series.
     * 
     * @param other The series from which the value is to be added.
     * @param index The index of the value to be added.
     */
    virtual void addFromSeries(const ISeries* other, size_t index) = 0;

    /**
     * @brief Clones the series.
     * 
     * @return A shared pointer to the cloned series.
     */
    virtual shared_ptr<ISeries> clone() const = 0;

};


/**
 * @brief A template class representing a series of data.
 * 
 * This class stores a series of data of type T and provides some operations
 * to manipulate and access the data.
 */
template<typename T>
class Series : public ISeries {
private:
    vector<T> data; /**< The vector storing the data of type T. */
    string name; /**< The name of the series. */

public:
    /**
     * @brief Constructs a new Series object with the given name.
     * 
     * @param name The name of the series.
     */
    Series(const string& name) : name(name) {}

    /**
     * @brief Destructor for the Series class.
     */
    ~Series() {
        data.clear();
    }

    /**
     * @brief Returns the name of the series.
     * 
     * @return The name of the series.
     */
    const string& getName() const {
        return name;
    }

    /**
     * @brief Sets the name of the series.
     * 
     * @param name The name of the series.
     */
    void setName(const string& name) {
        this->name = name;
    }

    /**
     * @brief Returns the size of the series.
     * 
     * @return The size of the series.
     */
    size_t size() const override {
        return data.size();
    }
    
    /**
     * @brief Returns the type information of the series.
     * 
     * @return The type information of the series.
     */
    const type_info& type() const override {
        return typeid(T);
    }

    /**
     * @brief Returns the data stored in the series.
     * 
     * @return The data stored in the series.
     */
    const vector<T>& getData() const {
        return data;
    }

    /**
     * @brief Adds a value to the series.
     * 
     * This function adds a value to the series after performing type checking.
     * If the type of the value does not match the type of the series, a
     * runtime_error is thrown.
     * 
     * @param value The value to be added to the series.
     * @throws runtime_error if the type of the value does not match the type of the series.
     */
    void add(const any& value) override {
        try {
            // Safely adding value to the series after type checking.
            const T& castedValue = any_cast<const T&>(value);
            data.push_back(castedValue);
        } catch (const bad_any_cast&) {
            // Handling the case where the type does not match.
            throw runtime_error("Type mismatch error: Unable to add value to Series " + name + " (expected " + string(type().name()) + ", received " + value.type().name() + ")");
        }
    }
    
    /**
     * @brief Adds a null value to the series, according to the type of the series.
     */
    void addNull() {
        data.push_back(T());
    }
    

    /**
     * @brief Removes the data at the specified index in the series.
     * 
     * @param index The index of the data to be removed.
     * @throws out_of_range if the index is out of range.
     */
    void removeAtIndex(size_t index) {
        if (index < data.size()) {
            data.erase(data.begin() + index);
        } else {
            throw out_of_range("Index out of range for Series removal.");
        }
    }

    /**
     * @brief Clears the series data.
     */
    void clear() override {
        data.clear();
    }

    /**
     * @brief Returns the value at the specified index in the series.
     * 
     * @param index The index of the value to be accessed.
     * @return The value at the specified index in the series.
     */
    const T& operator[](size_t index) const {
        return data[index];
    }

    /**
     * @brief Returns a reference to the value at the specified index in the series.
     * 
     * @param index The index of the value to be accessed.
     * @return A reference to the value at the specified index in the series.
     */
    T& operator[](size_t index) {
        return data[index];
    }

    /**
     * @brief Returns the data at the specified index in the series.
     * 
     * @param index The index of the data to be accessed.
     * @return The data at the specified index in the series.
     * @throws out_of_range if the index is out of range.
     */
    any getDataAtIndex(size_t index) const override {
        if (index >= data.size()) {
            throw out_of_range("Index out of range");
        }
        return data[index];
    }
    
    /**
     * @brief Returns the data at the specified index as a string.
     * 
     * If the type of the data is string, the data is returned as is.
     * For other types, the data is converted to a string using the convertToString function.
     * 
     * @param index The index of the data to be accessed.
     * @return The data at the specified index as a string.
     * @throws out_of_range if the index is out of range.
     */
    string getStringAtIndex(size_t index) const override {
        if (index >= data.size()) {
            throw out_of_range("Index out of range");
        }
        // Specialization for string to bypass to_string
        if constexpr (is_same<T, string>::value) {
            return data[index];
        } else {
            // Use to_string for basic types; might need customization for others
            return convertToString(data[index]);
        }
    }

    /**
     * @brief Computes the sum of the elements in the series.
     * 
     * This function computes the sum of the elements in the series. It is only available for arithmetic types.
     * 
     * @return The sum of the elements in the series.
     */
    any sum() const override {
        if constexpr (is_arithmetic<T>::value) {
            return accumulate(data.begin(), data.end(), T(0));
        } else {
            throw runtime_error("Sum operation not supported for non-arithmetic types.");
        }
    }

    /**
     * @brief Computes the mean of the elements in the series.
     * 
     * This function computes the mean of the elements in the series. It is only available for arithmetic types.
     * 
     * @return The mean of the elements in the series.
     */
    double mean() const override {
        if constexpr (is_arithmetic<T>::value) {
            return accumulate(data.begin(), data.end(), 0.0) / data.size();
        } else {
            throw runtime_error("Mean operation not supported for non-arithmetic types.");
        }
    }

    /**
     * @brief Generates a new series with unique values.
     * 
     * This function generates a new series with unique values from the current series.
     * 
     * @return A shared pointer to the new series with unique values.
     */
    shared_ptr<Series<T>> unique() const {
        unordered_set<T> seen;  // To keep track of seen values
        // Create a new series to store unique values
        shared_ptr<Series<T>> uniqueSeries = make_shared<Series<T>>(name + " (Unique)");

        // Iterate over the data and add unique values to the new series
        for (const auto& value : data) {
            if (seen.insert(value).second) {  // .second is true if the insert was successful (i.e., the value was not already present)
                uniqueSeries->add(value);
            }
        }

        return uniqueSeries;
    }

    /**
     * @brief Prints the series information to the standard output.
     * 
     * The series name, type, size, and data are printed to the standard output.
     */
    void print() const override {
        cout << endl << "----------------" << endl;
        cout << "Name: " << name << endl;
        cout << "Type: " << type().name() << endl;
        cout << "Size: " << size() << endl;
        cout << "Data: " << endl;
        cout << "################" << endl;
        for (const auto& value : data) {
            cout << any_cast<const T&>(value) << endl; // This requires that T is stream-insertable
        }
        cout << "----------------" << endl;
    }

    /**
     * @brief Adds a value to the series from another series.
     * 
     * This function adds a value to the series from another series at the specified index.
     * If the type of the other series does not match the type of the series, a
     * runtime_error is thrown.
     * 
     * @param other The series from which the value is to be added.
     * @param index The index of the value to be added.
     * @throws runtime_error if the type of the other series does not match the type of the series.
     */
    void addFromSeries(const ISeries* other, size_t index) override {
        const Series<T>* casted = dynamic_cast<const Series<T>*>(other);
        if (casted) {
            add(casted->getData()[index]);
        } else {
            throw runtime_error("Type mismatch between series");
        }
    }

    /**
     * @brief Clones the series.
     * 
     * @return A shared pointer to the cloned series.
     */
    shared_ptr<ISeries> clone() const override {
        auto clonedSeries = make_shared<Series<T>>(name);
        clonedSeries->data = data; // Deep copy the data
        return clonedSeries;
    }
};

#endif // SERIES_HPP

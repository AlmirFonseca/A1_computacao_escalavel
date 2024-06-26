#include "../src/Series.hpp"
#include <iostream>
#include <string>

using namespace std;

int main() {
    // Integer series
    Series<int> intSeries("Integer Series");
    try {
        intSeries.add(5);
        intSeries.add(10);
        intSeries.add(15); // Corrected to match type
        cout << "Third element in intSeries (using operator[]): " << intSeries[2] << endl;
    } catch (const runtime_error& e) {
        cout << "Caught an error in intSeries: " << e.what() << endl;
    }

    // Demonstrating setName and getName
    intSeries.setName("Renamed Integer Series");
    cout << "intSeries new name: " << intSeries.getName() << endl;

    // Double series
    Series<double> doubleSeries("Double Series");
    try {
        doubleSeries.add(3.14);
        doubleSeries.add(2.71);
        cout << "First element in doubleSeries (using operator[]): " << doubleSeries[0] << endl;
    } catch (const runtime_error& e) {
        cout << "Caught an error in doubleSeries: " << e.what() << endl;
    }

    // String series
    Series<string> stringSeries("String Series");
    try {
        stringSeries.add(string("Hello"));
        stringSeries.add(string("World"));
        cout << "Second element in stringSeries (using operator[]): " << stringSeries[1] << endl;
        cout << "Editing second element in stringSeries\n";
        stringSeries[1] = "Universe";
        cout << "Second element in stringSeries (using operator[]): " << stringSeries[1] << endl;
    } catch (const runtime_error& e) {
        cout << "Caught an error in stringSeries: " << e.what() << endl;
    }

    // Long long series
    Series<long long> longSeries("Long Series");
    try {
        longSeries.add(1713214692000);
        longSeries.add(1713214692001);
        longSeries.add(stoll(string("1713214692002"))); // Corrected to match type
        cout << "First element in longSeries (using operator[]): " << longSeries[0] << endl;
        cout << "Second element in longSeries (using operator[]): " << longSeries[1] << endl;
        cout << "Third element in longSeries (using operator[]): " << longSeries[2] << endl;
    } catch (const runtime_error& e) {
        cout << "Caught an error in longSeries: " << e.what() << endl;
    }

    // Using print() to display series info and data
    cout << "\nDisplaying intSeries info and data:\n";
    intSeries.print();

    cout << "\nDisplaying doubleSeries info and data:\n";
    doubleSeries.print();

    cout << "\nDisplaying stringSeries info and data:\n";
    stringSeries.print();

    cout << "\nDisplaying longSeries info and data:\n";
    longSeries.print();

    // Test the sum method
    cout << "\nSum of intSeries: " << any_cast<int>(intSeries.sum()) << endl;
    cout << "Sum of doubleSeries: " << any_cast<double>(doubleSeries.sum()) << endl;
    cout << "Sum of longSeries: " << any_cast<long long>(longSeries.sum()) << endl;
    // cout << "Sum of stringSeries: " << any_cast<string>(stringSeries.sum()) << endl; //-> Throws a runtime error
    cout << endl;

    // Test the mean method
    cout << "Mean of intSeries: " << (intSeries.mean()) << endl;
    cout << "Mean of doubleSeries: " << (doubleSeries.mean()) << endl;
    cout << "Mean of longSeries: " << (longSeries.mean()) << endl;
    // cout << "Mean of stringSeries: " << any_cast<double>(stringSeries.mean()) << endl; //-> Throws a runtime error
    cout << endl;

    Series<int> mySeries("Sample Series");
    mySeries.add(1);
    mySeries.add(2);
    mySeries.add(2);
    mySeries.add(3);
    mySeries.add(3);
    mySeries.add(3);
    mySeries.add(4);
    mySeries.add(4);
    mySeries.add(4);
    mySeries.add(4);

    auto uniqueSeries = mySeries.unique(); // Auto = shared_ptr<Series<int>>

    cout << "Original Series:" << endl;
    mySeries.print();

    cout << "Unique Series:" << endl;
    uniqueSeries->print();

    // Test the addNull method
    cout << "\nAdding a null element to intSeries\n";
    intSeries.addNull();
    cout << "Last element in intSeries (using operator[]): " << intSeries[intSeries.size() - 1] << endl;

    cout << "\nAdding a null element to doubleSeries\n";
    doubleSeries.addNull();
    cout << "Last element in doubleSeries (using operator[]): " << doubleSeries[doubleSeries.size() - 1] << endl;

    cout << "\nAdding a null element to stringSeries\n";
    stringSeries.addNull();
    cout << "Last element in stringSeries (using operator[]): " << stringSeries[stringSeries.size() - 1] << endl;

    cout << "\nAdding a null element to longSeries\n";
    longSeries.addNull();
    cout << "Last element in longSeries (using operator[]): " << longSeries[longSeries.size() - 1] << endl;

    return 0;
}

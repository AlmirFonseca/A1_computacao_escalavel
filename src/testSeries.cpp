#include "Series.hpp"
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

    // Using print() to display series info and data
    cout << "\nDisplaying intSeries info and data:\n";
    intSeries.print();

    cout << "\nDisplaying doubleSeries info and data:\n";
    doubleSeries.print();

    cout << "\nDisplaying stringSeries info and data:\n";
    stringSeries.print();

    try{
        doubleSeries.addString(string("1010.1"));
        cout << "Added string to doubleSeries\n";
        doubleSeries.print();
    } catch (const runtime_error& e) {
        cout << "Caught an error in doubleSeries: " << e.what() << endl;
    }

    return 0;
}

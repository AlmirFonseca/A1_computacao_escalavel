#include "DataFrame.hpp"
#include <iostream>

int main() {
    try {
        // Create a DataFrame with three columns
        DataFrame df({"ID", "Name", "Score"});

        // Add rows of data
        df.addRow(1, "Alice", 92.5);
        df.addRow(2, "Bob", 88.0);
        df.addRow(3, "Charlie", 79.5);

        // Print out the DataFrame information
        std::cout << "DataFrame has " << df.getColumnCount() << " columns and "
                  << df.getRowCount() << " rows." << std::endl;

        // Assuming DataFrame has a method to print its content
        // This part is pseudocode and needs actual implementation in DataFrame
        df.print();
        cout << endl;

        // Test deleting the column "Name"
        df.dropColumn("Name");
        df.print();
        cout << endl;

        // Test the method addColumn
        df.addColumn("Status", string("Off"));
        df.addColumn("Grade", 'F');
        df.addColumn("Value", 0);
        df.print();
        cout << endl;

        // Test the method dropRow over the second row (index 1)
        df.dropRow(1);
        df.print();
        cout << endl;

        df.addRow(2, 100.0, string("On"), 'A', 1);
        df.addRow(4, 100.0, string("On"), 'A', 1);
        df.print();
        cout << endl;

        // Test the method printColumnTypes
        df.printColumnTypes();
        cout << endl;

        // Test the method filterByColumn over the colum score == 100
        df.filterByColumn("ID", 1);
        df.print();
        cout << endl;

        df.addRow(5, 100.0, string("On"), 'A', 1);
        df.addRow(6, 100.0, string("On"), 'A', 0);
        df.addRow(7, 100.0, string("Off"), 'A', 1);
        df.addRow(8, 100.0, string("Off"), 'A', 0);
        df.print();
        cout << endl;

        cout << "KEEP FALSE" << endl;
        df.filterByColumn("Score", 100.0, false); // false means (remove the rows that are don't match)
        df.print();
        cout << endl;

        df.addRow(5, 100.0, string("On"), 'A', 1);
        df.addRow(6, 100.0, string("On"), 'A', 0);
        df.addRow(7, 100.0, string("Off"), 'A', 1);
        df.print();
        cout << endl;

        cout << "KEEP TRUE" << endl;
        df.filterByColumn("Score", 100.0, true); // true means (keep the rows that are match)
        df.print();
        cout << endl;
        df.addRow(8, 100.0, string("Off"), 'A', 0);

    } catch (const std::exception& e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
    }

    return 0;
}

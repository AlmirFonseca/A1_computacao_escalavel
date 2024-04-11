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

        // Test deleting the column "Name"
        df.dropColumn("Name");
        df.print();

        // Test the method addColumn
        df.addColumn("Status", string("Off"));
        df.addColumn("Grade", 'F');
        df.addColumn("Value", 0);
        df.print();

        // Test the method dropRow over the second row (index 1)
        df.dropRow(1);
        df.print();

    } catch (const std::exception& e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
    }

    return 0;
}

#include "../src/DataFrame.hpp"
#include <iostream>
#include <vector>

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

        // // Test the method filterByColumn over the colum score == 100
        // df.filterByColumn("ID", 1);
        // df.print();
        // cout << endl;

        // df.addRow(5, 100.0, string("On"), 'A', 1);
        // df.addRow(6, 100.0, string("On"), 'A', 0);
        // df.addRow(7, 100.0, string("Off"), 'A', 1);
        // df.addRow(8, 100.0, string("Off"), 'A', 0);
        // df.print();
        // cout << endl;

        // cout << "KEEP FALSE" << endl;
        // df.filterByColumn("Score", 100.0, false); // false means (remove the rows that are don't match)
        // df.print();
        // cout << endl;

        // df.addRow(5, 100.0, string("On"), 'A', 1);
        // df.addRow(6, 100.0, string("On"), 'A', 0);
        // df.addRow(7, 100.0, string("Off"), 'A', 1);
        // df.print();
        // cout << endl;

        // cout << "KEEP TRUE" << endl;
        // df.filterByColumn("Score", 100.0, true); // true means (keep the rows that are match)
        // df.print();
        // cout << endl;
        // df.addRow(8, 100.0, string("Off"), 'A', 0);

        DataFrame dfAge({"Age", "Salary", "Name"});
        
        // Adding some rows to the DataFrame
        dfAge.addRow(25, 50000, "John");
        dfAge.addRow(30, 60000, "Alice");
        dfAge.addRow(35, 70000, "Bob");
        dfAge.addRow(40, 80000, "Charlie");
        dfAge.addRow(45, 90000, "David");
        dfAge.addRow(50, 100000, "Emma");
        dfAge.addRow(55, 110000, "Frank");
        dfAge.addRow(60, 120000, "Grace");
        dfAge.addRow(65, 130000, "Henry");
        dfAge.addRow(70, 140000, "Isabella");
        dfAge.addRow(75, 150000, "Jack");
        dfAge.addRow(80, 160000, "Kate");
        dfAge.addRow(85, 170000, "Liam");
        dfAge.addRow(90, 180000, "Mia");
        dfAge.addRow(95, 190000, "Noah");
        dfAge.addRow(100, 200000, "Olivia");

        cout << "Original DataFrame (before filtering)" << endl;
        dfAge.print();
        
        // Filter rows where Age is less than 35
        dfAge.filterByColumn("Age", 35, CompareOperation::GREATER_THAN_OR_EQUAL);
        cout << "DataFrame after filtering (Age >= 35)" << endl;
        dfAge.print();

        // Filter rows where Salary is greater than 100000
        dfAge.filterByColumn("Salary", 100000, CompareOperation::LESS_THAN);
        cout << "DataFrame after filtering (Salary < 100000)" << endl;
        dfAge.print();

        // Filter rows where Name is not "Bob"
        dfAge.filterByColumn("Name", "Bob", CompareOperation::NOT_EQUAL);
        cout << "DataFrame after filtering (Name != Bob)" << endl;
        dfAge.print();

        // Create two DataFrames with same column names
        DataFrame df1({"timestamp", "sensor1", "sensor2", "origin"});
        DataFrame df2({"timestamp", "sensor1", "sensor2", "origin"});

        // Adding rows to df1
        df1.addRow(1, 10, 100.0, string("DataFrame1"));
        df1.addRow(3, 11, 110.0, string("DataFrame1"));
        df1.addRow(5, 12, 120.0, string("DataFrame1"));
        df1.addRow(7, 13, 130.0, string("DataFrame1"));
        df1.addRow(9, 14, 140.0, string("DataFrame1"));
        df1.print();
        cout << endl;

        // Adding rows to df2
        df2.addRow(2, 15, 150.0, string("Dataframe2"));
        df2.addRow(4, 16, 160.0, string("Dataframe2"));
        df2.addRow(6, 17, 170.0, string("Dataframe2"));
        df2.addRow(8, 18, 180.0, string("Dataframe2"));
        df2.addRow(10, 19, 190.0, string("Dataframe2"));
        df2.addRow(12, 20, 200.0, string("Dataframe2"));
        df2.addRow(14, 21, 210.0, string("Dataframe2"));
        df2.addRow(16, 22, 220.0, string("Dataframe2"));
        df2.addRow(18, 23, 230.0, string("Dataframe2"));
        df2.addRow(20, 24, 240.0, string("Dataframe2"));
        df2.print();
        cout << endl;

        // Merge the DataFrames ordered by the "timestamp" column
        DataFrame merged = DataFrame::mergeOrdered(df1, df2, "timestamp");

        // Print the merged DataFrame
        merged.print();
        cout << endl;

        // Test the deep copy method
        cout << "Testing the deep copy method" << endl;
        DataFrame dfCopy;
        dfCopy.deepCopy(df);

        cout << "Original DataFrame" << endl;
        df.print();
        cout << "Copy DataFrame" << endl;
        dfCopy.print();

        cout << "Let's change both DataFrames" << endl;
        df.addRow(100, 100.0, string("On"), 'A', 1);
        dfCopy.addRow(200, 200.0, string("Off"), 'B', 2);

        cout << "Original DataFrame" << endl;
        df.print();
        cout << "Copy DataFrame" << endl;
        dfCopy.print();

        cout << endl;

        cout << "Test the concat method" << endl;
        cout << "Original + copy DataFrame (deep copy)" << endl;
        DataFrame dfConcat = DataFrame::concat(df, dfCopy);
        dfConcat.print();

        cout << endl;

        cout << "Test a copy without keeping the data" << endl;
        DataFrame dfCopy2;
        dfCopy2.deepCopy(df, false);
        dfCopy2.printColumnTypes();
        cout << "Original DataFrame" << endl;
        df.print();
        cout << "Copy DataFrame" << endl;
        dfCopy2.print();

        cout << "Let's add a row to the copy DataFrame" << endl;
        dfCopy2.addRow(300, 300.0, string("On"), 'C', 3);
        cout << "Original DataFrame" << endl;
        df.print();
        cout << "Copy DataFrame" << endl;
        dfCopy2.print();

        vector<string> columnNames = {"ID", "Name", "Score"};

        DataFrame emptyDataframe = DataFrame(columnNames);

        emptyDataframe.printColumnTypes();

        emptyDataframe.addColumnValue(0, 4);
        emptyDataframe.addColumnValue(1, "Diana");
        emptyDataframe.addColumnValue(2, 100.0);
        emptyDataframe.increaseRowCount();

        emptyDataframe.print();
        emptyDataframe.printColumnTypes();

        cout << endl;
        cout << "Testing the method getDataType" << endl;
        cout << "1 as " <<  emptyDataframe.getDataType("1") << endl;
        cout << "1.2 as " <<  emptyDataframe.getDataType("1.2") << endl;
        cout << "1713214692000 as " <<  emptyDataframe.getDataType("1713214692000") << endl;
        cout << "c as " <<  emptyDataframe.getDataType("c") << endl;
        cout << "string as " <<  emptyDataframe.getDataType("string") << endl;
        cout << endl;

        DataFrame plainDataFrame = DataFrame({"ID", "Timestamp", "Score", "Name"});
        plainDataFrame.addRow("1", "1713214692000", "100.0", "Alice");
        plainDataFrame.addRow("2", "1713214693000", "90.0", "Bob");
        plainDataFrame.addRow("3", "1713214694000", "80.8", "Charlie");
        plainDataFrame.print();
        plainDataFrame.printColumnTypes();

        // Testing the method getValueAt
        cout << endl;
        cout << "Testing the method getValueAt" << endl;
        cout << "Value at row 0 and column 0: " << plainDataFrame.getValueAt(1, 1) << endl;

        // Test the sum method
        cout << "\nSum of Score: " << any_cast<float>(plainDataFrame.sum("Score")) << endl;




    } catch (const std::exception& e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
    }

    return 0;
}
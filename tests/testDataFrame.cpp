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
        cout << "DataFrame has " << df.getColumnCount() << " columns and "
                  << df.getRowCount() << " rows." << endl;

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
        DataFrame dfCopy = DataFrame::deepCopy(df, true);

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
        DataFrame dfCopy2 = DataFrame::deepCopy(df, false);
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

        // Test the valueCounts method
        cout << "\nOriginal Dataframe: " << endl;
        DataFrame dfForCounting({"ID", "Age"});
        dfForCounting.addRow(1, 20);
        dfForCounting.addRow(2, 22);
        dfForCounting.addRow(3, 20);
        dfForCounting.addRow(4, 22);
        dfForCounting.addRow(5, 20);
        dfForCounting.addRow(6, 22);
        dfForCounting.addRow(7, 20);
        dfForCounting.addRow(8, 21);
        dfForCounting.addRow(9, 21);
        dfForCounting.addRow(10, 21);
        dfForCounting.addRow(11, 21);
        dfForCounting.addRow(12, 21);
        dfForCounting.print();

        cout << "Value counts of Name: " << endl;
        DataFrame valueCounts = dfForCounting.valueCounts("Age");
        valueCounts.print();

        // Test the sortByColumn method
        cout << "\nOriginal Dataframe: " << endl;
        DataFrame dfForSorting({"ID", "Age", "Name"});
        dfForSorting.addRow(1, 20, "John");
        dfForSorting.addRow(2, 22, "Alice");
        dfForSorting.addRow(3, 20, "Bob");
        dfForSorting.addRow(4, 22, "Charlie");
        dfForSorting.addRow(5, 20, "David");
        dfForSorting.addRow(6, 22, "Emma");
        dfForSorting.addRow(7, 20, "Frank");
        dfForSorting.addRow(8, 21, "Grace");
        dfForSorting.addRow(9, 21, "Henry");
        dfForSorting.addRow(10, 21, "Isabella");

        dfForSorting.print();

        cout << "Sorting by Age: " << endl;
        dfForSorting.sortByColumn("Age");
        dfForSorting.print();

        cout << "Sorting by Name: " << endl;
        dfForSorting.sortByColumn("Name");
        dfForSorting.print();

        cout << endl;
        cout << "Testing the left join method" << endl;

        DataFrame dfToJoin1({"ID", "Name", "Job"});
        dfToJoin1.addRow(1, "Alice", "Engineer");
        dfToJoin1.addRow(2, "Bob", "Doctor");
        dfToJoin1.addRow(3, "Charlie", "Teacher");
        dfToJoin1.addRow(4, "David", "Teacher");
        dfToJoin1.addRow(5, "Emma", "Engineer");
        dfToJoin1.addRow(6, "Frank", "Doctor");
        dfToJoin1.addRow(7, "Grace", "Youtuber");

        DataFrame dfToJoin2({"Job", "Salary"});
        dfToJoin2.addRow("Engineer", 100000);
        dfToJoin2.addRow("Doctor", 150000);
        dfToJoin2.addRow("Teacher", 80000);

        // Perform the left join
        DataFrame joinedDf = dfToJoin1.leftJoin(dfToJoin2, "Job");

        // Print the result
        cout << "Left DataFrame: " << endl;
        dfToJoin1.print();
        cout << "Right DataFrame: " << endl;
        dfToJoin2.print();
        cout << "Joined DataFrame: " << endl;
        joinedDf.print();
        joinedDf.printColumnTypes();

        // Create two DataFrames with ID and Value columns
        DataFrame dfToMergeAndSum1({"ID", "Value"});
        DataFrame dfToMergeAndSum2({"ID", "Value"});

        // Add some rows to df1
        dfToMergeAndSum1.addRow("A", 100);
        dfToMergeAndSum1.addRow("B", 150);
        dfToMergeAndSum1.addRow("C", 200);

        // Add some rows to df2
        dfToMergeAndSum2.addRow("A", 300);
        dfToMergeAndSum2.addRow("B", 250);
        dfToMergeAndSum2.addRow("D", 450);

        // Merge and sum the two DataFrames
        DataFrame mergedAndSumResult = DataFrame::mergeAndSum(dfToMergeAndSum1, dfToMergeAndSum2, "ID", "Value");

        // Print the result
        cout << "Resulting DataFrame after merging and summing:" << endl;
        mergedAndSumResult.print();

        // Create a DataFrame with ID and Timestamp columns
        DataFrame dfToGetMean({"ID", "Timestamp"});
        dfToGetMean.addRow("A", 1715958895599);
        dfToGetMean.addRow("B", 1715958895600);
        dfToGetMean.addRow("C", 1715958895601);
        dfToGetMean.addRow("D", 1715958895602);
        dfToGetMean.addRow("E", 1715958895603);
        dfToGetMean.addRow("F", 1715958895604);

        // Get the sum of the Timestamp column
        long long sum = any_cast<long long>(dfToGetMean.sum("Timestamp"));
        cout << "Sum of Timestamp column: " << sum << endl;

        // Get the mean of the Timestamp column
        double mean = dfToGetMean.mean("Timestamp");
        // Get the integer part of the mean
        long long meanInt = static_cast<long long>(mean);
        cout << "Mean of Timestamp column: " << meanInt << endl;

    } catch (const exception& e) {
        cerr << "Exception occurred: " << e.what() << endl;
    }

    return 0;
}
#include <iostream>
#include <thread>
#include <mutex>
#include "../src/DataHandler.hpp"
#include "../src/DataFrame.hpp"
#include "../src/Queue.hpp"
#include "../src/DataRepo.hpp"

// Example of using the data handler in a separate thread
int main() {
    // Creation of input and output queues
    Queue<DataFrame*> queueSelect(15);
    Queue<DataFrame*> queueFilter(15);

    DataRepo* repo = new DataRepo();
    repo->setExtractionStrategy("csv");
    string csv_location = "../src/test.csv";
    DataFrame* df = repo->extractData(csv_location);

    // Print DataFrame information
    df->print();

    // Push the DataFrame to the input queue
    queueSelect.push(df);

    auto filter = FilterHandler(&queueSelect, &queueFilter);
    
    // Start the filter in a separate thread
    filter.filterByColumn("Score", 100, CompareOperation::GREATER_THAN);

    df->print();

    return 0;
}
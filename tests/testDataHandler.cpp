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

    
    // thest JoinHandler
    Queue<DataFrame*> queueProdBuy(10);
    Queue<DataFrame*> queueJoin2(15);
    vector<Queue<DataFrame*>*> outputQueuesJoin = {&queueProdBuy};

    // Create a JoinHandler object
    JoinHandler joinHandler(&queueProdBuy, outputQueuesJoin);
    joinHandler.join(*df, "extra_2");

    // Print the DataFrame
    queueProdBuy.pop()->print();

    


    




    return 0;
}
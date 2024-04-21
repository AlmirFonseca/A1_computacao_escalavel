#include <iostream>
#include "../src/TimerTrigger.hpp"
#include "../src/Queue.hpp"
#include "../src/DataRepo.hpp"
#include "../src/DataHandler.hpp"
#include <chrono>
#include <thread>
#include <memory>

using namespace std;


int main(int argc, char* argv[]) {
    DataRepo* repo = new DataRepo();
    repo->setExtractionStrategy("txt");
    string csv_location = "../mock/mock_files/log/";
    DataFrame* df = repo->extractData(csv_location + "1log_simulation.txt", ';');
    DataFrame* df2 = repo->extractData(csv_location + "2log_simulation.txt", ';');

    // create a queue of dataframes
    Queue<DataFrame*> queue(10);
    queue.push(df);
    queue.push(df2);

    // Create another queue of dataframes
    Queue<DataFrame*> queue2(10);

    // Create a handler for the queue
    FilterHandler handler(&queue, &queue2);

    handler.filterByColumn("content", 137934928, CompareOperation::LESS_THAN_OR_EQUAL);

    Queue<DataFrame*> queue3(10);

    FilterHandler handler2(&queue2, &queue3);

    handler2.filterByColumn("content", 121301069, CompareOperation::GREATER_THAN);
    
    repo->setLoadStrategy("csv");
    int i = 0;
    while (!queue3.isEmpty()) {
        DataFrame* df = queue3.pop();
        df->print();
        repo->setExtractDf(df);
        repo->setLoadFileName("filtered" + to_string(i) + ".csv");
        i++;
    }

    Trigger* trigger = new TimerTrigger(std::chrono::seconds(5));
    trigger->addObserver(std::make_shared<DataRepo>(*repo));

    trigger->activate();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    trigger->deactivate();

    return 0;
}


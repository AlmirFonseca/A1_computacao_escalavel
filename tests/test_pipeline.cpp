#include <iostream>
#include "../src/TimerTrigger.hpp"
#include "../src/Queue.hpp"
#include "../src/DataRepo.hpp"
#include "../src/DataHandler.hpp"
#include "../src/ThreadPool.hpp"
#include <chrono>
#include <thread>
#include <memory>

using namespace std;


int main(int argc, char* argv[]) {
    DataRepo* repo = new DataRepo();
    repo->setExtractionStrategy("txt");
    string csv_location = "../mock/mock_files/log/";

    // create a queue of dataframes
    Queue<DataFrame*> queueIn0(10);
    Queue<DataFrame*> queueIn1(10);

    for (int i = 1; i <= 10; i++) {
        DataFrame* df = repo->extractData(csv_location + to_string(i) + "log_simulation.txt", ';');
        queueIn0.push(df);

        // deep copy of the dataframe
        DataFrame* df_copy = new DataFrame;
        *df_copy = DataFrame::deepCopy(*df);
        queueIn1.push(df_copy);
    }

    // Create another queue of dataframes
    Queue<DataFrame*> queueUser(10);
    FilterHandler handler(&queueIn0, &queueUser);

    Queue<DataFrame*> queueView(10);
    FilterHandler handler2(&queueUser, &queueView);

    Queue<DataFrame*> queueCountView(10);
    CountLinesHandler handler3(&queueView, &queueCountView);

    Queue<DataFrame*> queueAudi(10);
    FilterHandler handler01(&queueIn1, &queueAudi);

    Queue<DataFrame*> queueProdView(10);
    ValueCountHandler ProdView(&queueView, &queueProdView);


    
    // Create a thread pool with 4 threads
    ThreadPool pool(4);

    // Add tasks to the thread pool
    pool.addTask([&handler]() {
        handler.filterByColumn("type", string("User"), CompareOperation::EQUAL);
    });

    pool.addTask([&handler2]() {
        handler2.filterByColumn("extra_1", string("ZOOM"), CompareOperation::EQUAL);
    });

    pool.addTask([&handler3]() {
        handler3.countLines();
    });

    pool.addTask([&handler01]() {
        handler01.filterByColumn("type", string("Audit"), CompareOperation::EQUAL);
    });

    pool.addTask([&ProdView]() {
        ProdView.valueCount("extra_2");
    });


    while (queueCountView.isEmpty() || queueProdView.isEmpty()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    repo->setLoadStrategy("csv");
    int i = 0;
    DataFrame* df_main = queueCountView.pop();
    while (!queueCountView.isEmpty()) {
        DataFrame* df = queueCountView.pop();
        df_main->concat(*df);
    }

    // Sum the DataFrame
    int sum = any_cast<int>(df_main->sum("count"));

    // Create a new DataFrame with the sum
    DataFrame* sumDf = new DataFrame({"sum"});

    sumDf->addRow(sum);

    repo->setExtractDf(sumDf);
    repo->setLoadFileName("output1.csv");

    DataFrame* df_main2 = queueProdView.pop();
    while (!queueProdView.isEmpty()) {
        DataFrame* df = queueProdView.pop();
        df_main2->concat(*df);
    }

    df_main2->print();


    Trigger* trigger = new TimerTrigger(std::chrono::seconds(5));
    trigger->addObserver(std::make_shared<DataRepo>(*repo));

    trigger->activate();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    trigger->deactivate();

    return 0;
}


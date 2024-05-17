#include <iostream>
#include "TimerTrigger.hpp"
#include "RequestTrigger.hpp"
#include "ETL.hpp"
#include "Observer.hpp"
#include "Queue.hpp"
#include "DataRepo.hpp"
#include "DataHandler.hpp"
#include "ThreadPool.hpp"
#include <chrono>
#include <thread>
#include <memory>


using namespace std;

int process(Queue<DataFrame*>* queueCA, int maxQueueSize, int numThreads){
    // Create a thread pool with 8 threads
    ThreadPool pool(numThreads);

    //========= USING ONLY DATA FROM "CADE ANALYTICS"

    // Duplicate the dataframes in queueCA to send to the two different pipelines
    Queue<DataFrame*> queueCA1(maxQueueSize);
    Queue<DataFrame*> queueCA2(maxQueueSize);
    vector<Queue<DataFrame*>*> outputQueuesCA = {&queueCA1, &queueCA2};
    CopyHandler copyCA(queueCA, outputQueuesCA);
    pool.addTask([&copyCA]() {
        copyCA.copy();
    });


    // Número de produtos visualizados por minuto:
    Queue<DataFrame*> queueUser(maxQueueSize);
    vector<Queue<DataFrame*>*> outputQueuesUser = {&queueUser};
    FilterHandler filterUser(&queueCA1, outputQueuesUser);
    pool.addTask([&filterUser]() {
        filterUser.filterByColumn("type", string("User"), CompareOperation::EQUAL);
    });

    Queue<DataFrame*> queueView(maxQueueSize);
    Queue<DataFrame*> queueView1(maxQueueSize);
    vector<Queue<DataFrame*>*> outputQueuesView = {&queueView, &queueView1};
    FilterHandler filterView(&queueUser, outputQueuesView);
    pool.addTask([&filterView]() {
        filterView.filterByColumn("extra_1", string("ZOOM"), CompareOperation::EQUAL);
    });

    Queue<DataFrame*> queueCountView(maxQueueSize);
    vector<Queue<DataFrame*>*> outputQueuesCountView = {&queueCountView};
    CountLinesHandler CountView(&queueView, outputQueuesCountView);
    pool.addTask([&CountView]() {
        CountView.countLines();
    });
    

    // Número de produtos comprados por minuto:
    Queue<DataFrame*> queueAuditoria(maxQueueSize);
    vector<Queue<DataFrame*>*> outputQueuesAuditoria = {&queueAuditoria};
    FilterHandler FilterAuditoria(&queueCA2, outputQueuesAuditoria);
    pool.addTask([&FilterAuditoria]() {
        FilterAuditoria.filterByColumn("type", string("Audit"), CompareOperation::EQUAL);
    });


    Queue<DataFrame*> queueBuy(maxQueueSize);
    Queue<DataFrame*> queueBuy1(maxQueueSize);
    Queue<DataFrame*> queueBuy2(maxQueueSize);
    vector<Queue<DataFrame*>*> outputQueuesBuy = {&queueBuy, &queueBuy1};
    FilterHandler filterBuy(&queueAuditoria, outputQueuesBuy);
    pool.addTask([&filterBuy]() {
        filterBuy.filterByColumn("extra_1", string("BUY"), CompareOperation::EQUAL);
    });

    Queue<DataFrame*> queueCountBuy(maxQueueSize);
    vector<Queue<DataFrame*>*> outputQueuesCountBuy = {&queueCountBuy};
    CountLinesHandler CountBuy(&queueBuy, outputQueuesCountBuy);
    pool.addTask([&CountBuy]() {
        CountBuy.countLines();
    });


    // Número de usuários únicos visualizando cada produto por minuto
    Queue<DataFrame*> queueProdView(maxQueueSize);
    Queue<DataFrame*> queueProdView1(maxQueueSize);
    vector<Queue<DataFrame*>*> outputQueuesProdView = {&queueProdView, &queueProdView1};
    ValueCountHandler ProdView(&queueView1, outputQueuesProdView);
    pool.addTask([&ProdView]() {
        ProdView.countByColumn("extra_2");
    });



    // Ranking de produtos mais comprados na última hora
    Queue<DataFrame*> queueProdBuy(maxQueueSize);
    vector<Queue<DataFrame*>*> outputQueuesProdBuy = {&queueProdBuy};
    ValueCountHandler ProdBuy(&queueBuy1, outputQueuesProdBuy);
    pool.addTask([&ProdBuy]() {
        ProdBuy.countByColumn("extra_2");
    });

    Queue<DataFrame*> queueBuyRanking(maxQueueSize);
    vector<Queue<DataFrame*>*> outputQueuesBuyRanking = {&queueBuyRanking};
    SortHandler SortBuy(&queueProdBuy, outputQueuesBuyRanking);
    pool.addTask([&SortBuy]() {
        SortBuy.sortByColumn("Count");
    });


    // Ranking de produtos mais visualizados na última hora
    Queue<DataFrame*> queueViewRanking(maxQueueSize);
    vector<Queue<DataFrame*>*> outputQueuesViewRanking = {&queueViewRanking};
    SortHandler SortView(&queueProdView1, outputQueuesViewRanking);
    pool.addTask([&SortView]() {
        SortView.sortByColumn("Count");
    });

    // Quantidade média de visualizações de um produto antes de efetuar uma compra
    // Queue<DataFrame*> queueViewBuy(maxQueueSize);
    // vector<Queue<DataFrame*>*> outputQueuesViewBeforeBuy = {&queueViewBuy};
    // JoinHandler JoinViewBuy(&queueView1, outputQueuesViewBeforeBuy);
    // // add task to thread pool
    // pool.addTask([&JoinViewBuy, &queueBuy1]() { // Capture queueBuy1 in the lambda capture list
    //     JoinViewBuy.join(*queueBuy1.pop(), "extra_1");
    // });


    // Número de produtos vendidos sem disponibilidade no estoque
    // Queue<DataFrame*> queueBuyStock(maxQueueSize);
    // vector<Queue<DataFrame*>*> outputQueuesBuyStock = {&queueBuyStock};
    // JoinHandler JoinBuyStock(&queueBuy2, outputQueuesBuyStock);
    // // add task to thread pool
    // pool.addTask([&JoinBuyStock, &queueCV]() { // Capture queueCV in the lambda capture list
    //     JoinBuyStock.join(*queueCV.pop(), "extra_2");
    // });

    vector<Queue<DataFrame*>*> outputQueuesPipeline = {&queueCountView, &queueCountBuy, &queueProdView, &queueBuyRanking, &queueViewRanking};

    DataFrame* result_dataframes[5] = {nullptr, nullptr, nullptr, nullptr, nullptr};
    mutex result_mutexes[5];
    DataFrame* dataframe_times[5] = {nullptr, nullptr, nullptr, nullptr, nullptr};

    // Tasks to merge the dataframes in the output queues into the result dataframes
    for (int i = 0; i < 5; i++) {
        DataFrame** result_dataframe = &result_dataframes[i];
        DataFrame** dataframe_time = &dataframe_times[i];
        Queue<DataFrame*>* outputQueue = outputQueuesPipeline[i];
        mutex* result_mutex = &result_mutexes[i];

        pool.addTask([outputQueue, result_dataframe, dataframe_time, result_mutex]() {
            // Wait for the output queue to have data
            if (outputQueue->isEmpty()) return;

            
            // Merge the dataframes in the output queue with the result dataframe
            while (!outputQueue->isEmpty()) {
                DataFrame* df = outputQueue->pop();

                {
                    lock_guard<mutex> lock(*result_mutex);

                    // If the result dataframe is empty, set it to the first dataframe in the queue
                    if (*result_dataframe == nullptr) 
                    {
                        *result_dataframe = df;
                        
                        // Create a new dataframe with the time difference of the first dataframe
                        *dataframe_time = new DataFrame({"time"});

                        long long timestamp = (*result_dataframe)->getTimestamp();
                        long long current_timestamp = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
                        
                        // Add the time difference to the dataframe beetwen current timestamp and its timestamp
                        (*dataframe_time)->addRow(current_timestamp - timestamp);
                    }
                    else 
                    {
                        // If the dataframe has only one column, sum the values else merge the dataframes
                        if (df->getColumnCount() == 1) **result_dataframe = DataFrame::mergeAndSum(**result_dataframe, *df, "", "Count");
                        else **result_dataframe = DataFrame::mergeAndSum(**result_dataframe, *df, "Value", "Count");
                        
                        // Add the time difference to the dataframe beetwen current timestamp and its timestamp
                        long long timestamp = df->getTimestamp();
                        long long current_timestamp = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
                        (*dataframe_time)->addRow(current_timestamp - timestamp);

                        // Delete the old DataFrame
                        delete df;
                    }
                }
            }
        });
    }

    // Vector of file names and which trigger activates them
    vector<string> fileNames = {"CountView.csv", "CountBuy.csv", "ProdView.csv", "BuyRanking.csv", "ViewRanking.csv"};
    vector<string> triggeredBy = {"Min", "Min", "Min", "Hour", "Hour"};

    // Triggers to activate the DataRepos
    int MIN = 5;
    int HOUR = 10;

    Trigger* triggerMin = new TimerTrigger(std::chrono::seconds(MIN));
    Trigger* triggerHour = new TimerTrigger(std::chrono::seconds(HOUR));


    // ------------------THE ERROR IS HERE------------------
    // Create a DataRepo for each pipeline
    for (int i = 0; i < 5; i++) {
        // Create a DataRepo for each result dataframe
        DataRepo* dataRepo = new DataRepo();
        dataRepo->setExtractDf(&result_dataframes[i], &result_mutexes[i]);
        dataRepo->setLoadStrategy("csv");
        dataRepo->setLoadFileName("../processed/" + fileNames[i]);

        // Create a DataRepo for each time dataframe
        DataRepo* dataRepoTime = new DataRepo();
        dataRepoTime->setExtractDf(&dataframe_times[i], &result_mutexes[i]);
        dataRepoTime->setLoadStrategy("csv");
        dataRepoTime->setLoadFileName("../processed/times_" + fileNames[i]);

        // Set the trigger for each pipeline
        if (triggeredBy[i] == "Min") {
            triggerMin->addObserver(std::make_shared<DataRepo>(*dataRepo));
            triggerMin->addObserver(std::make_shared<DataRepo>(*dataRepoTime));
        } else if (triggeredBy[i] == "Hour") {
            triggerHour->addObserver(std::make_shared<DataRepo>(*dataRepo));
            triggerHour->addObserver(std::make_shared<DataRepo>(*dataRepoTime));
        }
    }

    
    // Activate the triggers
    triggerHour->activate();
    triggerMin->activate();

    while (true) {
        std::this_thread::sleep_for(std::chrono::hours(1));
    }

    triggerHour->deactivate();
    triggerMin->deactivate();

    return 0;
}


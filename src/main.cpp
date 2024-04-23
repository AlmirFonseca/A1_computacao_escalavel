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

int main(int argc, char* argv[]) {

    const std::string csvDirPath = "../mock/mock_files/csv";
    const std::string txtDirPath = "../mock/mock_files/log";
    const std::string requestDirPath = "../mock/mock_files/request";

    const int DEFAULT_INPUT_QUEUE_SIZE = 100;
    const int DEFAULT_OUTPUT_QUEUE_SIZE = 100;
    const int DEFAULT_MAX_THREADS = 10;

    // Check if arguments are provided
    int inputQueueSize = DEFAULT_INPUT_QUEUE_SIZE;
    int outputQueueSize = DEFAULT_OUTPUT_QUEUE_SIZE;
    int maxThreads = DEFAULT_MAX_THREADS;

    if (argc >= 4) {
        // Assuming arguments are provided in the order: inputQueueSize, outputQueueSize, maxThreads
        inputQueueSize = std::stoi(argv[1]);
        outputQueueSize = std::stoi(argv[2]);
        maxThreads = std::stoi(argv[3]);
    }

    // create a queue of dataframes
    Queue<DataFrame*> queueCV(inputQueueSize);
    Queue<DataFrame*> queueDC(inputQueueSize);
    Queue<DataFrame*> queueCA(inputQueueSize);


    ETL etl(csvDirPath, txtDirPath, requestDirPath, queueCV, queueDC, queueCA);

    // Create a thread pool with 8 threads
    ThreadPool pool(8);


    // Número de produtos visualizados por minuto:
    Queue<DataFrame*> queueUser(10);
    vector<Queue<DataFrame*>*> outputQueuesUser = {&queueUser};
    FilterHandler filterUser(&queueDC, outputQueuesUser);
    pool.addTask([&filterUser]() {
        filterUser.filterByColumn("type", string("User"), CompareOperation::EQUAL);
    });

    Queue<DataFrame*> queueView(10);
    Queue<DataFrame*> queueView1(10);
    vector<Queue<DataFrame*>*> outputQueuesView = {&queueView, &queueView1};
    FilterHandler filterView(&queueUser, outputQueuesView);
    pool.addTask([&filterView]() {
        filterView.filterByColumn("extra_1", string("ZOOM"), CompareOperation::EQUAL);
    });

    Queue<DataFrame*> queueCountView(10);
    vector<Queue<DataFrame*>*> outputQueuesCountView = {&queueCountView};
    CountLinesHandler CountView(&queueView, outputQueuesCountView);
    pool.addTask([&CountView]() {
        CountView.countLines();
    });
    

    // Número de produtos comprados por minuto:
    Queue<DataFrame*> queueAuditoria(10);
    vector<Queue<DataFrame*>*> outputQueuesAuditoria = {&queueAuditoria};
    FilterHandler FilterAuditoria(&queueDC, outputQueuesAuditoria);
    pool.addTask([&FilterAuditoria]() {
        FilterAuditoria.filterByColumn("type", string("Audit"), CompareOperation::EQUAL);
    });

    Queue<DataFrame*> queueBuy(10);
    Queue<DataFrame*> queueBuy1(10);
    Queue<DataFrame*> queueBuy2(10);
    vector<Queue<DataFrame*>*> outputQueuesBuy = {&queueBuy, &queueBuy1, &queueBuy2};
    FilterHandler filterBuy(&queueAuditoria, outputQueuesBuy);
    pool.addTask([&filterBuy]() {
        filterBuy.filterByColumn("extra_1", string("BUY"), CompareOperation::EQUAL);
    });

    Queue<DataFrame*> queueCountBuy(10);
    vector<Queue<DataFrame*>*> outputQueuesCountBuy = {&queueCountBuy};
    CountLinesHandler CountBuy(&queueBuy, outputQueuesCountBuy);
    pool.addTask([&CountBuy]() {
        CountBuy.countLines();
    });


    // Número de usuários únicos visualizando cada produto por minuto
    Queue<DataFrame*> queueProdView(10);
    Queue<DataFrame*> queueProdView1(10);
    vector<Queue<DataFrame*>*> outputQueuesProdView = {&queueProdView, &queueProdView1};
    ValueCountHandler ProdView(&queueView1, outputQueuesProdView);
    pool.addTask([&ProdView]() {
        ProdView.countByColumn("extra_2");
    });



    // Ranking de produtos mais comprados na última hora
    Queue<DataFrame*> queueProdBuy(10);
    vector<Queue<DataFrame*>*> outputQueuesProdBuy = {&queueProdBuy};
    ValueCountHandler ProdBuy(&queueBuy1, outputQueuesProdBuy);
    pool.addTask([&ProdBuy]() {
        ProdBuy.countByColumn("extra_2");
    });

    Queue<DataFrame*> queueBuyRanking(10);
    vector<Queue<DataFrame*>*> outputQueuesBuyRanking = {&queueBuyRanking};
    SortHandler SortBuy(&queueProdBuy, outputQueuesBuyRanking);
    pool.addTask([&SortBuy]() {
        SortBuy.sortByColumn("Count");
    });


    // Ranking de produtos mais visualizados na última hora
    Queue<DataFrame*> queueViewRanking(10);
    vector<Queue<DataFrame*>*> outputQueuesViewRanking = {&queueViewRanking};
    SortHandler SortView(&queueProdView1, outputQueuesViewRanking);
    pool.addTask([&SortView]() {
        SortView.sortByColumn("Count");
    });

    // Quantidade média de visualizações de um produto antes de efetuar uma compra
    // Queue<DataFrame*> queueViewBuy(10);
    // JoinHandler JoinViewBuy(&queueViewRanking, &queueBuyRanking, &queueViewBuy);

    // Número de produtos vendidos sem disponibilidade no estoque
    // Queue<DataFrame*> queueBuyStock(10);
    // vector<Queue<DataFrame*>*> outputQueuesBuyStock = {&queueBuyStock};
    // JoinHandler JoinBuyStock(&queueBuy2, outputQueuesBuyStock);

    vector<Queue<DataFrame*>*> outputQueuesPipeline = {&queueCountView, &queueCountBuy, &queueProdView, &queueBuyRanking, &queueViewRanking};

    DataFrame* result_dataframes[5] = {nullptr, nullptr, nullptr, nullptr, nullptr};

    for (int i = 0; i < 5; i++) {
        if (outputQueuesPipeline[i]->isEmpty())
        {
            continue;
        }
        result_dataframes[i] = outputQueuesPipeline[i]->pop();
        while (!outputQueuesPipeline[i]->isEmpty())
        {
            DataFrame* df = outputQueuesPipeline[i]->pop();

            // If the dataframe has only one column, sum the values
            if (df->getColumnCount() == 1){
                *result_dataframes[i] = DataFrame::mergeAndSum(*result_dataframes[i], *df, "", "Count");
            }

            // If the dataframe has two columns, merge the dataframes and sum the values
            else{
                *result_dataframes[i] = DataFrame::mergeAndSum(*result_dataframes[i], *df, "Value", "Count");
            }

        }
        // string filename = "output" + to_string(i+1) + ".csv";
        // repoRead->loadData(result_dataframes[i], filename);
    }
    
    for (int i = 0; i < 5; i++) {
        DataFrame** result_dataframe = &result_dataframes[i];
        Queue<DataFrame*>* outputQueue = outputQueuesPipeline[i];
        pool.addTask([outputQueue, result_dataframe]() {
            if (outputQueue->isEmpty())
            {
                return;
            }
            if (*result_dataframe == nullptr)
            {
                *result_dataframe = outputQueue->pop();
            }
            if (!outputQueue->isEmpty())
            {
                DataFrame* df = outputQueue->pop();
                // If the dataframe has only one column, sum the values
                if (df->getColumnCount() == 1){
                    **result_dataframe = DataFrame::mergeAndSum(**result_dataframe, *df, "", "Count");
                }

                // If the dataframe has two columns, merge the dataframes and sum the values
                else{
                    **result_dataframe = DataFrame::mergeAndSum(**result_dataframe, *df, "Value", "Count");
                }

            }
        });
    }

    DataRepo* repoCountView = new DataRepo();
    repoCountView->setExtractDf(&result_dataframes[0]);
    repoCountView->setLoadStrategy("csv");
    repoCountView->setLoadFileName("../processed/CountView.csv");

    DataRepo* repoCountBuy = new DataRepo();
    repoCountBuy->setExtractDf(&result_dataframes[1]);
    repoCountBuy->setLoadStrategy("csv");
    repoCountBuy->setLoadFileName("../processed/CountBuy.csv");

    DataRepo* repoProdView = new DataRepo();
    repoProdView->setExtractDf(&result_dataframes[2]);
    repoProdView->setLoadStrategy("csv");
    repoProdView->setLoadFileName("../processed/ProdView.csv");

    DataRepo* repoBuyRanking = new DataRepo();
    repoBuyRanking->setExtractDf(&result_dataframes[3]);
    repoBuyRanking->setLoadStrategy("csv");
    repoBuyRanking->setLoadFileName("../processed/BuyRanking.csv");

    DataRepo* repoViewRanking = new DataRepo();
    repoViewRanking->setExtractDf(&result_dataframes[4]);
    repoViewRanking->setLoadStrategy("csv");
    repoViewRanking->setLoadFileName("../processed/ViewRanking.csv");


    Trigger* triggerMin = new TimerTrigger(std::chrono::seconds(5));
    triggerMin->addObserver(std::make_shared<DataRepo>(*repoCountView));
    triggerMin->addObserver(std::make_shared<DataRepo>(*repoCountBuy));
    triggerMin->addObserver(std::make_shared<DataRepo>(*repoProdView));

    Trigger* triggerHour = new TimerTrigger(std::chrono::seconds(10));
    triggerHour->addObserver(std::make_shared<DataRepo>(*repoBuyRanking));
    triggerHour->addObserver(std::make_shared<DataRepo>(*repoViewRanking));

    TimerTrigger timer(std::chrono::seconds(1));
    RequestTrigger request(std::chrono::seconds(1), std::chrono::seconds(3));

    timer.addObserver(std::make_shared<ETL>(etl));
    request.addObserver(std::make_shared<ETL>(etl));

    timer.activate();
    request.activate();
    triggerHour->activate();
    triggerMin->activate();
    
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    timer.deactivate();
    request.deactivate();
    triggerHour->deactivate();
    triggerMin->deactivate();

    return 0;
}


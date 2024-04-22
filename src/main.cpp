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
    Queue<DataFrame*> queueIn0(10);
    Queue<DataFrame*> queueIn1(10);

    // ref para vector de dataframes (conta verde)
    std::vector<DataFrame*> dataframes;

    // pass queueIn0 and queueIn1 to the ETL object as reference (so it can be changed when the triggers are activated)
    ETL etl(csvDirPath, txtDirPath, requestDirPath, DEFAULT_INPUT_QUEUE_SIZE, DEFAULT_OUTPUT_QUEUE_SIZE, DEFAULT_MAX_THREADS, queueIn0, queueIn1, dataframes);
    // ref para o conta verde e duas filas: uma para o cadeanalitics e outra pro datacat
    // cada arquivo um dataframe diferente

    /// fazere primeiro a coluna 

    TimerTrigger timer(std::chrono::seconds(1));
    RequestTrigger request(std::chrono::seconds(2), std::chrono::seconds(5));

    timer.addObserver(std::make_shared<ETL>(etl));
    request.addObserver(std::make_shared<ETL>(etl));

    timer.activate();
    request.activate();
    

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    timer.deactivate();
    request.deactivate();

    return 0;
}


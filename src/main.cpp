#include <iostream>
#include "TimerTrigger.hpp"
#include "RequestTrigger.hpp"
#include "ETL.hpp"
#include "Observer.hpp"
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

    ETL etl(csvDirPath, txtDirPath, requestDirPath, DEFAULT_INPUT_QUEUE_SIZE, DEFAULT_OUTPUT_QUEUE_SIZE, DEFAULT_MAX_THREADS);

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


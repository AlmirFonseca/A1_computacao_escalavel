#include <iostream>
#include "TimerTrigger.hpp"
#include "RequestTrigger.hpp"
#include "Pipeline.hpp"
#include "Observer.hpp"
#include <chrono>
#include <thread>
#include <memory>

using namespace std;

int main() {

    // print a message
    cout << "Hello, World!" << endl;

    const std::string csvDirPath = "../mock/mock_files/csv";
    const std::string txtDirPath = "../mock/mock_files/log";
    const std::string requestDirPath = "../mock/mock_files/request";

    Pipeline pipeline(csvDirPath, txtDirPath, requestDirPath);

    TimerTrigger timer(std::chrono::seconds(1));
    RequestTrigger request(std::chrono::seconds(2), std::chrono::seconds(5));

    timer.addObserver(std::make_shared<Pipeline>(pipeline));
    request.addObserver(std::make_shared<Pipeline>(pipeline));

    timer.activate();
    request.activate();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    timer.deactivate();
    request.deactivate();

    return 0;
}


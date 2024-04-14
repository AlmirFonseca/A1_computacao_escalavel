#include <iostream>
#include "TimerTrigger.hpp"
#include "RequestTrigger.hpp"
#include "Pipeline.hpp"
using namespace std;

int main() {

    Pipeline pipeline;
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


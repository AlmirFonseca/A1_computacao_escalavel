#include <iostream>
#include "TimerTrigger.hpp"
#include "RequestTrigger.hpp"
using namespace std;

int main() {
    // Example usage
    TimerTrigger timer(std::chrono::seconds(1));
    RequestTrigger request(std::chrono::seconds(2), std::chrono::seconds(5));

    timer.activate();
    request.activate();

    std::this_thread::sleep_for(std::chrono::seconds(10));

    timer.deactivate();
    request.deactivate();

    return 0;
}

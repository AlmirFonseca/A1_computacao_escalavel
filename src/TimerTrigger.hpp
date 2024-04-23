#ifndef TIMERTRIGGER_HPP
#define TIMERTRIGGER_HPP

#include <iostream>
#include <chrono>
#include <thread>
#include "AbstractTrigger.hpp"
using namespace std;

// TimerTrigger class
/**
 * @brief Trigger activated at regular intervals.
 * 
 * This class represents a trigger that activates at regular intervals.
 */
class TimerTrigger : public Trigger {
private:
    std::chrono::milliseconds interval;
    bool active;

public:
    /**
     * @brief Constructs a new TimerTrigger object with a given interval.
     * 
     * @param interval The interval between activations.
     */
    TimerTrigger(std::chrono::milliseconds interval) : interval(interval), active(false) {}

    /**
     * @brief Sets the interval between activations.
     * 
     * @param newInterval The new interval.
     */
    void setInterval(std::chrono::milliseconds newInterval) {
        interval = newInterval;
    }

    // Overriding activate method
    void activate() override {
        active = true;
        std::thread([this]() {
            while (active) {
                for (auto observer : observers) {
                    observer->updateOnTimeTrigger(); // Notify all observers
                }
                std::this_thread::sleep_for(interval);
            }
        }).detach();
    }


    void deactivate() override {
        active = false;
    }
};

#endif // TIMERTRIGGER_HPP

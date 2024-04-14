#ifndef REQUESTTRIGGER_HPP
#define REQUESTTRIGGER_HPP

#include <iostream>
#include <chrono>
#include <thread>
#include <random>
#include "AbstractTrigger.hpp"
using namespace std;

// RequestTrigger class
/**
 * @brief Trigger activated randomly with a specified interval range.
 * 
 * This class represents a trigger that activates randomly within a specified interval range.
 */
class RequestTrigger : public Trigger {
private:
    std::chrono::milliseconds minInterval;
    std::chrono::milliseconds maxInterval;
    bool active;

public:
    /**
     * @brief Constructs a new RequestTrigger object with a given interval range.
     * 
     * @param minInterval The minimum interval between activations.
     * @param maxInterval The maximum interval between activations.
     */
    RequestTrigger(std::chrono::milliseconds minInterval, std::chrono::milliseconds maxInterval)
        : minInterval(minInterval), maxInterval(maxInterval), active(false) {}

    // Overriding activate method
    void activate() override {
        active = true;
        std::thread([this]() {
            while (active) {
                std::chrono::milliseconds interval = getRandomInterval();
                std::this_thread::sleep_for(interval);
                std::cout << "RequestTrigger activated!" << std::endl;
                // Notify all observers
                for (auto observer : observers) {
                    observer->handleRequest();
                }
            }
        }).detach();
    }

    // Overriding deactivate method
    void deactivate() override {
        active = false;
    }

private:
    // Method to generate random interval
    std::chrono::milliseconds getRandomInterval() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(minInterval.count(), maxInterval.count());
        return std::chrono::milliseconds(distrib(gen));
    }
};

#endif // REQUESTTRIGGER_HPP

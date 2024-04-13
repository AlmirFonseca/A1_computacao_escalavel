#include <iostream>
#include <chrono>
#include <thread>
#include <random>
using namespace std;

// Abstract class Trigger
/**
 * @brief Abstract base class for triggers.
 * 
 * This class defines the interface for triggers.
 */
class Trigger {
public:
    /**
     * @brief Activates the trigger.
     */
    virtual void activate() = 0;

    /**
     * @brief Deactivates the trigger.
     */
    virtual void deactivate() = 0;
};

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
                std::this_thread::sleep_for(interval);
                std::cout << "TimerTrigger activated!" << std::endl;
            }
        }).detach();
    }

    // Overriding deactivate method
    void deactivate() override {
        active = false;
    }
};

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

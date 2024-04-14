#ifndef ABSTRACTTRIGGER_HPP
#define ABSTRACTTRIGGER_HPP

#include <iostream>
#include <vector>
#include <memory> // For std::shared_ptr
#include "Observer.hpp"

using namespace std;


// Abstract class Trigger
/**
 * @brief Abstract base class for triggers.
 * 
 * This class defines the interface for triggers.
 */
class Trigger {
protected:
    std::vector<std::shared_ptr<Observer>> observers;

public:
    /**
     * @brief Activates the trigger.
     */
    virtual void activate() = 0;

    /**
     * @brief Deactivates the trigger.
     */
    virtual void deactivate() = 0;

    /**
     * @brief Adds an observer to the trigger.
     * 
     * @param observer The observer to add.
     */
     // Method to add observer
    void addObserver(std::shared_ptr<Observer> observer) {
        observers.push_back(observer);
    }
};

#endif // ABSTRACTTRIGGER_HPP

#ifndef ABSTRACTTRIGGER_HPP
#define ABSTRACTTRIGGER_HPP

#include <iostream>
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

#endif // ABSTRACTTRIGGER_HPP

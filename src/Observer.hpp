# ifndef OBSERVER_HPP
# define OBSERVER_HPP

#include <iostream>
#include <vector>

// Define the Observer class, which implements the Observer pattern
class Observer {
public:
    virtual void updateOnTimeTrigger() = 0;
    virtual void updateOnRequestTrigger() = 0;

};

# endif
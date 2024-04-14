# ifndef OBSERVER_HPP
# define OBSERVER_HPP

#include <iostream>
#include <vector>

// Define the Observer class, which implements the Observer pattern
class Observer {
public:
    virtual void update() = 0;
    virtual void handleRequest() = 0;

};

# endif
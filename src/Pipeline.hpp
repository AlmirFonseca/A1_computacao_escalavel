#include <iostream>
#include <vector>
#include <chrono>
#include "Observer.hpp"

// createe pipeline class inheriting from observer
class Pipeline : public Observer {

private:
    std::vector<int> data;

    void readCSV() {
        std::cout << "Reading data from CSV file..." << std::endl;
    }

    void readHTTP() {
        std::cout << "Reading data from HTTP request..." << std::endl;
    }

public:
    Pipeline() {
        std::cout << "Pipeline created!" << std::endl;
    }

    ~Pipeline() {
        std::cout << "Pipeline destroyed!" << std::endl;
    }

    // Interface for notification (update) from triggers
    void update() override {
        readCSV();
        // sleep
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "CSV Pipeline ended!" << std::endl;
    }

    // Interface for handling request from triggers
    void handleRequest() override {
        readHTTP();
        // sleep
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Request Pipeline ended!" << std::endl;
    }

};

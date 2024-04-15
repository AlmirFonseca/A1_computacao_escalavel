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

    std::vector<std::string> processedFiles;
    
    // Function to monitor the folders and process new files
    void monitorFolders(const std::string& folderPath) {
        for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
            if (fs::is_regular_file(entry.path())) {
                std::string filePath = entry.path().string();
                if (std::find(processedFiles.begin(), processedFiles.end(), filePath) == processedFiles.end()) {
                    // File hasn't been processed yet
                    if (filePath.find("/csv/") != std::string::npos) {
                        readCSV(filePath);
                    } else if (filePath.find("/request/") != std::string::npos) {
                        readHTTP(filePath);
                    }
                    processedFiles.push_back(filePath);
                }
            }
        }
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

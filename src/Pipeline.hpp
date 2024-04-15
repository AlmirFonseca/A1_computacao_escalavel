#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <thread>
#include <filesystem>
#include "Observer.hpp"

class Pipeline : public Observer {
private:
    std::vector<std::string> processedFiles;

    void readCSV(const std::string& filePath) {
        std::cout << "Reading data from CSV file: " << filePath << std::endl;
        // ...
    }

    void readHTTP(const std::string& filePath) {
        std::cout << "Reading data from HTTP request file: " << filePath << std::endl;
        // ...
    }

    void processFile(const std::string& filePath) {
        std::cout << "Processing file: " << filePath << std::endl;
        
        // Extract the folder name from the filePath
        std::string folderName = filePath.substr(0, filePath.find_last_of("/"));
        folderName = folderName.substr(folderName.find_last_of("/") + 1);

        if (folderName == "csv") {
            readCSV(filePath);
        } else if (folderName == "request") {
            reaHTTP(filePath);
        } else {
            std::cout << "Unknown folder: " << folderName << std::endl;
        }
    }

    // Function to monitor the folders and process new files
    void monitorFolders(const std::string& folderPath) {
        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (std::filesystem::is_regular_file(entry.path())) {
                const std::string& filePath = entry.path().string();
                if (std::find(processedFiles.begin(), processedFiles.end(), filePath) == processedFiles.end()) {
                    // File hasn't been processed yet
                    processFile(filePath);
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
        monitorFolders("mock/mock_files/csv");
        // sleep
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "CSV Pipeline ended!" << std::endl;
    }

    // Interface for handling request from triggers
    void handleRequest() override {
        monitorFolders("mock/mock_files/request");
        // sleep
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Request Pipeline ended!" << std::endl;
    }
};

#endif // PIPELINE_HPP

#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <thread>
#include <filesystem>
#include <dirent.h>
#include <sys/stat.h>
#include <string>
#include <mutex>  // for std::mutex
#include "Observer.hpp"

class Pipeline : public Observer {
private:
    std::vector<std::string> processedCSVFiles;
    std::vector<std::string> processedTXTFiles;
    std::vector<std::string> processedRequestFiles;
    std::string csvDirPath;
    std::string txtDirPath;
    std::string requestDirPath;

    bool isRegularFile(const std::string& path) {
        struct stat statbuf;
        if (stat(path.c_str(), &statbuf) == 0) {
            return S_ISREG(statbuf.st_mode);
        }
        return false;
    }

    // Function to monitor the directory and return new files
    std::vector<std::string> monitorDirectory(const std::string& dirPath, std::vector<std::string>& processedFiles) {
        std::vector<std::string> newFiles;

        // Get the list of files in the directory
        DIR *directory = opendir(dirPath.c_str());
        if (directory == nullptr) {
            std::cerr << "Failed to open directory: " << dirPath << std::endl;
            return newFiles;
        }

        struct dirent *entry;
        while ((entry = readdir(directory)) != nullptr) {
            std::string fullPath = dirPath + "/" + entry->d_name;
            if (isRegularFile(fullPath) && std::find(processedFiles.begin(), processedFiles.end(), fullPath) == processedFiles.end()) {
                newFiles.push_back(fullPath);
            }
        }
        closedir(directory);

        return newFiles;
    }

    void processFiles(const std::vector<std::string>& files, std::vector<std::string>& processedFiles) {
        if (files.empty()) {
            std::cout << "No new files found." << std::endl;
            return;
        }

        for (const auto& filePath : files) {
            std::cout << "Processing new file: " << filePath << std::endl;
            processedFiles.push_back(filePath);
            
            // Extract the folder name from the filePath
            std::string folderName = filePath.substr(0, filePath.find_last_of("/"));
            folderName = folderName.substr(folderName.find_last_of("/") + 1);

            // Add your file processing logic here
            // Example: readCSV(filePath), readHTTP(filePath)
        }
    }

public:
    Pipeline(const std::string& csvDirectory, const std::string& txtDirectory, const std::string& requestDirectory) 
        : csvDirPath(csvDirectory), txtDirPath(txtDirectory), requestDirPath(requestDirectory) {
        std::cout << "Pipeline created!" << std::endl;
    }

    ~Pipeline() {
        std::cout << "Pipeline destroyed!" << std::endl;
    }

    // Interface for notification (update) from triggers
    void updateOnTimeTrigger() override {
        std::vector<std::string> newCSVFiles = monitorDirectory(csvDirPath, processedCSVFiles);
        processFiles(newCSVFiles, processedCSVFiles);

        std::vector<std::string> newTXTFiles = monitorDirectory(txtDirPath, processedTXTFiles);
        processFiles(newTXTFiles, processedTXTFiles);

        // sleep
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Time-triggered Pipeline ended!" << std::endl;
    }

    // Interface for handling request from triggers
    void updateOnRequestTrigger() override {
        std::vector<std::string> newRequestFiles = monitorDirectory(requestDirPath, processedRequestFiles);
        processFiles(newRequestFiles, processedRequestFiles);

        // sleep
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Request-triggered Pipeline ended!" << std::endl;
    }
};

#endif // PIPELINE_HPP

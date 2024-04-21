#ifndef ETL_HPP
#define ETL_HPP

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
#include "DataFrame.hpp"
#include "DataRepo.hpp"
#include "Queue.hpp"
#include "DataHandler.hpp"

class ETL : public Observer {
private:

    std::vector<std::string> processedCSVFiles;
    std::vector<std::string> processedTXTFiles;
    std::vector<std::string> processedRequestFiles;
    std::string csvDirPath;
    std::string txtDirPath;
    std::string requestDirPath;
    const int INPUT_MAX_QUEUE_SIZE;
    const int OUTPUT_MAX_QUEUE_SIZE;
    const int MAX_THREADS;

    // create dataRepo object that should be live throughout the pipeline
    DataRepo repo;

    bool isRegularFile(const std::string& path) {
        struct stat statbuf;
        if (stat(path.c_str(), &statbuf) == 0) {
            return S_ISREG(statbuf.st_mode);
        }
        return false;
    }

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

    std::vector<DataFrame> processFiles(const std::vector<std::string>& files, std::vector<std::string>& processedFiles) {
        if (files.empty()) {
            std::cout << "No new files found." << std::endl;
            return {};
        }

        std::vector<DataFrame> dataframes;

        for (const auto& filePath : files) {
            std::cout << "Processing new file: " << filePath << std::endl;
            processedFiles.push_back(filePath);
            
            repo.setExtractionStrategy("csv");
            cout<<"Extracting data from file: " + filePath + "\n";
            DataFrame* df = repo.extractData(filePath, ';');
            if (df == nullptr) {
                std::cerr << "DataFrame is empty!" << filePath<<std::endl;
                continue;
            }
            dataframes.push_back(*df);
        }
        return dataframes;
    }

    // get the csv files from the directory
    std::vector<std::string> getCSVFiles(const std::string& dirPath) {
        std::vector<std::string> csvFiles = {dirPath+"/products.csv", dirPath+"/purchase_orders.csv", dirPath+"/stock", dirPath+"/users.csv"};
        return csvFiles;
    }

    void procccessCsvPipeline(){
        std::vector<std::string> updatedCSVFiles = getCSVFiles(csvDirPath);
        std::vector<DataFrame> dataframesCSV = processFiles(updatedCSVFiles, processedCSVFiles);

        // apply filter to the dataframes

    }

    void procccessTxtPipeline(){
        std::vector<std::string> updatedTXTFiles = monitorDirectory(txtDirPath, processedTXTFiles);
        std::vector<DataFrame> dataframesTXT = processFiles(updatedTXTFiles, processedTXTFiles);

        // Create queue for the dataframes
        Queue<DataFrame*> inputQueue(INPUT_MAX_QUEUE_SIZE);
        Queue<DataFrame*> outputQueue(OUTPUT_MAX_QUEUE_SIZE);

        // apply filter to the dataframer, first create a datahandler object which will filter by type == "User"
        FilterHandler filterHandler(&inputQueue, &outputQueue);
        FilterHandler filterHandler2(&inputQueue, &outputQueue);
        
    }

    void processRequestPipeline(){
        std::vector<std::string> updatedRequestFiles = monitorDirectory(requestDirPath, processedRequestFiles);
        std::vector<DataFrame> dataframesRequest = processFiles(updatedRequestFiles, processedRequestFiles);
    }

public:
    ETL(const std::string& csvDirectory, const std::string& txtDirectory, const std::string& requestDirectory, const int INPUT_MAX_QUEUE_SIZE, const int OUTPUT_MAX_QUEUE_SIZE, const int MAX_THREADS) 
        : csvDirPath(csvDirectory), txtDirPath(txtDirectory), requestDirPath(requestDirectory), INPUT_MAX_QUEUE_SIZE(INPUT_MAX_QUEUE_SIZE), OUTPUT_MAX_QUEUE_SIZE(OUTPUT_MAX_QUEUE_SIZE), MAX_THREADS(MAX_THREADS) {
        std::cout << "ETL created!" << std::endl;
    }

    ~ETL() {
        std::cout << "ETL destroyed!" << std::endl;
    }

    // Interface for notification (update) from triggers
    void updateOnTimeTrigger() override {

        // procccessCsvPipeline();
        // create a thread for the csv pipeline
        std::thread csvThread(&ETL::procccessCsvPipeline, this);
        
        // procccessTxtPipeline();
        // create a thread for the txt pipeline
        std::thread txtThread(&ETL::procccessTxtPipeline, this);

        // join the threads
        csvThread.join();
        txtThread.join();

        // sleep
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Time-triggered Pipeline ended!" << std::endl;
    }

    // Interface for handling request from triggers
    void updateOnRequestTrigger() override {
     
        processRequestPipeline();   
        
        // sleep
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Request-triggered Pipeline ended!" << std::endl;
    }
};

#endif // ETL_HPP

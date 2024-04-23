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

    // create dataRepo object that should be live throughout the pipeline
    DataRepo repo;

    // queues references for the dataframes
    Queue<DataFrame*>& queueOutDC;
    Queue<DataFrame*>& queueOutCA;
    // reference pointer for vector of dataframes
    Queue<DataFrame*>& queueOutCV;
    

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

    void processFiles(const std::vector<std::string>& files, std::vector<std::string>& processedFiles, Queue<DataFrame*>& queueOut, string strategy) {
        if (files.empty()) {
            // std::cout << "No new files found." << std::endl;
            return;
        }

        for (const auto& filePath : files) {    
            // Check if the file is already processed
            if (std::find(processedFiles.begin(), processedFiles.end(), filePath) != processedFiles.end()) {
                continue;
            }

            processedFiles.push_back(filePath);
            
            repo.setExtractionStrategy(strategy);
            // cout<<"Extracting data from file: " + filePath + "\n";
            DataFrame* df = repo.extractData(filePath, ';');
            if (df == nullptr) {
                // std::cerr << "DataFrame is empty!" << filePath<<std::endl;
                continue;
            }
            queueOut.push(df);
        }
    }

    // get the csv files from the directory
    std::vector<std::string> getCSVFiles(const std::string& dirPath) {
        std::vector<std::string> csvFiles = {dirPath+"/products.csv", dirPath+"/purchase_orders.csv", dirPath+"/stock", dirPath+"/users.csv"};
        return csvFiles;
    }

    void procccessCsvPipeline(){
        std::vector<std::string> updatedCSVFiles = getCSVFiles(csvDirPath);
        processFiles(updatedCSVFiles, processedCSVFiles, queueOutCV, "csv");
    }

    void procccessTxtPipeline(){
        std::vector<std::string> updatedTXTFiles = monitorDirectory(txtDirPath, processedTXTFiles);
        processFiles(updatedTXTFiles, processedTXTFiles, queueOutDC, "txt");
    }

    void processRequestPipeline(){
        std::vector<std::string> updatedRequestFiles = monitorDirectory(requestDirPath, processedRequestFiles);
        processFiles(updatedRequestFiles, processedRequestFiles, queueOutCA, "txt");
    }

    public:
        ETL(const std::string& csvDirectory, const std::string& txtDirectory, const std::string& requestDirectory, Queue<DataFrame*>& queueCV, Queue<DataFrame*>& queueDC, Queue<DataFrame*>& queueCA)
            : csvDirPath(csvDirectory), txtDirPath(txtDirectory), requestDirPath(requestDirectory), queueOutCV(queueCV), queueOutDC(queueDC), queueOutCA(queueCA) {
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

    }

    // Interface for handling request from triggers
    void updateOnRequestTrigger() override {
     
        processRequestPipeline();   
        
    }
};

#endif // ETL_HPP

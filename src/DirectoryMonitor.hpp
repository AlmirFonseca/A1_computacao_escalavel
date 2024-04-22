// creates a class that receives a path and overrides Observer. It has a method that should return the new files that appeared on the directory since the last time it was called. 

#include "Observer.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <mutex>  // for std::mutex
#include <dirent.h>
#include <sys/stat.h>


class DirectoryMonitor : public Observer {
private:
    std::mutex mutex;
    std::vector<std::string> prevFiles;
    std::string dirPath;

    bool isRegularFile(const std::string& path) {
        struct stat statbuf;
        if (stat(path.c_str(), &statbuf) == 0) {
            return S_ISREG(statbuf.st_mode);
        }
        return false;
    }

public:
    // Public constructor
    DirectoryMonitor(const std::string& dir) : dirPath(dir) {}

    // Delete copy constructor and assignment operator
    DirectoryMonitor(const DirectoryMonitor&) = delete;
    DirectoryMonitor& operator=(const DirectoryMonitor&) = delete;

    std::vector<std::string> getNewFiles() {
        std::lock_guard<std::mutex> lock(mutex);  // Acquire mutex lock

        std::vector<std::string> newFiles;
        std::vector<std::string> currentFiles;

        // Get the list of files in the directory
        DIR *directory = opendir(dirPath.c_str());
        if (directory == nullptr) {
            std::cerr << "Failed to open directory" << std::endl;
            return newFiles;
        }

        struct dirent *entry;
        while ((entry = readdir(directory)) != nullptr) {
            std::string fullPath = dirPath + "/" + entry->d_name;
            if (isRegularFile(fullPath)) {
                currentFiles.push_back(entry->d_name);
            }
        }
        closedir(directory);

        // Find new files by comparing current files with previous files
        std::set_difference(currentFiles.begin(), currentFiles.end(),
                            prevFiles.begin(), prevFiles.end(),
                            std::back_inserter(newFiles));

        // Update previous files list
        prevFiles = currentFiles;

        return newFiles;
    }

    void updateOnTimeTrigger() override {
        std::vector<std::string> newFiles = getNewFiles();
        for (const auto& file : newFiles) {
            std::cout << "New file appeared: " << file << std::endl;
        }
    }

    void updateOnRequestTrigger() override {
        std::vector<std::string> newFiles = getNewFiles();
        for (const auto& file : newFiles) {
            std::cout << "New file appeared: " << file << std::endl;
        }
    }
    
};
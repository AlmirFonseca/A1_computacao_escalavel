#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <algorithm>
#include <vector>

using namespace std::chrono_literals;

std::vector<std::string> getExistingFiles(const std::string& path) {
  std::vector<std::string> files;
  for (const auto& entry : std::filesystem::directory_iterator(path)) {
    if (!entry.is_directory()) {
      files.push_back(entry.path().filename().string());
    }
  }
  return files;
}

void monitorNewFiles(const std::string& path) {
  std::vector<std::string> existingFiles = getExistingFiles(path);

  while (true) {
    std::this_thread::sleep_for(4s);
    std::vector<std::string> newFiles = getExistingFiles(path);

    // Compare new files with existing ones
    std::vector<std::string> detectedNewFiles;
    std::set_difference(newFiles.begin(), newFiles.end(), existingFiles.begin(), existingFiles.end(),
                        std::back_inserter(detectedNewFiles));

    if (!detectedNewFiles.empty()) {
      std::cout << "New files detected:" << std::endl;
      for (const auto& file : detectedNewFiles) {
        std::cout << file << std::endl;
      }
    }

    // Update existing files list
    existingFiles = newFiles;
  }
}

int main() {
  std::string folderPath = "../mock/mock_files/csv/";
  std::cout<<"Monitoring folder: "<<folderPath;

  monitorNewFiles(folderPath);
  return 0;
}

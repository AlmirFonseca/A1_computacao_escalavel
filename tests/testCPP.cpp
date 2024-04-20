#include<iostream>

#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <algorithm>
#include <vector>

using namespace std::chrono_literals;

// std::vector<std::string> getExistingFiles(const std::string& path) {
//   std::vector<std::string> files;
//   for (const auto& entry : std::filesystem::directory_iterator(path)) {
//     if (!entry.is_directory()) {
//       files.push_back(entry.path().filename().string());
//     }
//   }
//   return files;
// }

int main() {
    if (__cplusplus == 202101L) std::cout << "C++23";
    else if (__cplusplus == 202002L) std::cout << "C++20";
    else if (__cplusplus == 201703L) std::cout << "C++17";
    else if (__cplusplus == 201402L) std::cout << "C++14";
    else if (__cplusplus == 201103L) std::cout << "C++11";
    else if (__cplusplus == 199711L) std::cout << "C++98";
    else std::cout << "pre-standard C++." << __cplusplus;
    std::cout << "\n";
}
#include <iostream>
#include <fstream>
#include <string>
#include <limits> // Include this header for numeric_limits
#include <thread> // Include this header for sleep_for


class FileReader {
public:
    void read(const std::string& filename, int start, int & end) {
        std::ifstream file(filename); // Open the file
        if (!file.is_open()) { // Check if file opening failed
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }

        // Move to the start line
        for (int i = 1; i < start; ++i) {
            if (!file.ignore(std::numeric_limits<std::streamsize>::max(), '\n')) {
                end = i - 1; // Update end if start line is beyond EOF
                file.close(); // Close the file
                return;
            }
        }

        std::string line;
        int lineCount = start; // Initialize line counter
        while (std::getline(file, line)) {
            // Process the line here
            std::cout << line << std::endl; 
            ++lineCount; 
        }

        end = lineCount - 1; 
        file.close();
    }
};

int main() {
    FileReader reader;
    std::string filename = "example.txt";
    int startLine = 1; // Start reading from line 1 initially
    int endLine;

    // Loop to continuously read from the file
    while (true) {
        // Read from the file
        reader.read(filename, startLine, endLine);

        // Check if endLine is greater than startLine, indicating successful read
        if (endLine > startLine) {
            startLine = endLine + 1; // Set the start line for the next read
        } else {
            // No more lines to read, wait for some time before trying again
            std::cout << "No more lines to read. Waiting for 1 second before retrying." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        // wait for a second before reading again
        // writes on the file filename 5 lines more
        std::ofstream file(filename, std::ios_base::app);
        for (int i = 0; i < 5; ++i) {
            file << "New line " << i << std::endl;
        }
        file.close();
        
        // Sleep for a second before reading again
        std::this_thread::sleep_for(std::chrono::seconds(2));


    }

    return 0;
}

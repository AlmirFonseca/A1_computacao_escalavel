#include <iostream>
#include <fstream>
#include <string>
#include <limits> // Include this header for numeric_limits
#include <thread> // Include this header for sleep_for
#include "../src/DataRepo.hpp"

int main() {
    DataRepo* repo = new DataRepo();
    repo->setExtractionStrategy("csv");
    DataFrame* df_main = repo->extractData("./example.csv", ';');

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        int numRowsRead = df_main->getRowCount();
        DataFrame* df = repo->extractData("./example.csv", ';', numRowsRead+1);
        
        if (df->getRowCount() > 0) {
            df_main->concat(*df);
            df_main->print();
        }

    }
    
    return 0;
}

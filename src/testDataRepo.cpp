#include "DataRepo.hpp"
#include <iostream>

int main() {
    try {
        // Create a DataRepo object with a CsvExtractionStrategy
        DataRepo* repo = new DataRepo();
        repo->setExtractionStrategy("csv");
        DataFrame* df = repo->extractData("test.csv");
        df->print();

        // Load the data from the DataFrame into a txt file
        repo->setLoadStrategy("txt");
        repo->loadData(*df);

    } catch (const std::exception& e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
    }

    return 0;
}
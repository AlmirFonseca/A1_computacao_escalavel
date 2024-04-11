#include "DataRepo.hpp"
#include <iostream>

int main() {
    try {
        // Create a DataRepo object with a CsvExtractionStrategy
        DataRepo* repo = new DataRepo();
        repo->setExtractionStrategy("csv");

        DataFrame* df = repo->extractData("text.csv");
        df->print();

    } catch (const std::exception& e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
    }

    return 0;
}
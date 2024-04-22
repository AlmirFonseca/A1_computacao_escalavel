#include "../src/DataRepo.hpp"
#include <iostream>

int main() {
    try {
        // Create a DataRepo object with a CsvExtractionStrategy
        DataRepo* repo = new DataRepo();
        repo->setExtractionStrategy("csv");
        string csv_location = "../mock/mock_files/csv/";
        csv_location += "products.csv";
        DataFrame* df = repo->extractData(csv_location, ';');
        df->print();
        cout << "\nColumn types:\n";
        df->printColumnTypes();

        // Load the data from the DataFrame into a txt file
        repo->setLoadStrategy("txt");
        repo->loadData(*df);

    } catch (const std::exception& e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
    }

    return 0;
}
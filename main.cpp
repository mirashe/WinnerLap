#include "recordsReader.h"
#include "lapCalculator.h"

#include <iostream>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Error: Incorrect number of argument are given!" << std::endl;
        std::cerr << "Please pass the <kart times file path> as the argument in command line." << std::endl;

        return 1;
    }

    recordsReader reader;
    
    if (!reader.open(argv[1])) {
        std::cerr << "Error: Cannot open the given file: " << argv[1] << std::endl;

        return 1;
    }

    lapCalculator calculator(reader);

    try {
        auto [bestLapKartNumber, bestLapNumber] = calculator.getBestLap();

        std::cout << "Best lap kart number : " << bestLapKartNumber << std::endl;
        std::cout << "Best lap number: " << bestLapNumber << std::endl;
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
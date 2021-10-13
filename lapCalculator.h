#pragma once
#include "recordsReader.h"

#include <utility>

class lapCalculator {
public:
    lapCalculator(recordsReader& reader);

    // This function returns the number of all karts participated in race as the first output
    // And the maximum number of times an individual kart passed the start/finish line.
    // This is equal to number of laps plus one
    auto getNumberOfKartsAndPasses(const std::vector<timeRecord>& records);

    std::pair<size_t, size_t> getBestLap();

private:
    recordsReader& mReader;
};


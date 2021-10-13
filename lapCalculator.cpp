#include "lapCalculator.h"

#include <tuple>

lapCalculator::lapCalculator(recordsReader& reader) : mReader(reader) {
}

auto lapCalculator::getNumberOfKartsAndPasses(const std::vector<timeRecord>& records) {
    std::vector<size_t> kartLastLaps{};
    size_t lapsCount{};

    for (const auto& record : records) {
        if (record.mKartNumber > kartLastLaps.size())
            kartLastLaps.resize(record.mKartNumber);

        ++kartLastLaps[record.mKartNumber - 1];

        if (kartLastLaps[record.mKartNumber - 1] > lapsCount)
            lapsCount = kartLastLaps[record.mKartNumber - 1];
    }

    return std::tuple{ kartLastLaps.size(), lapsCount };
}

std::pair<size_t, size_t> lapCalculator::getBestLap()
{
    auto records = mReader.getRecords();
    auto [kartsCount, passesCount] = getNumberOfKartsAndPasses(records);

    size_t bestLapKartNumber{};
    auto bestLapTime = std::chrono::system_clock::duration::max();
    size_t bestLapNumber{};

    // to keep it simple I assumed kart numbers always start from 1 and continues sequentially,
    // otherwise we can use a map instead of vector
    std::vector<std::pair<size_t, recordClock>> passNumberAndTimes(kartsCount); // pass number & its time for each kart
    size_t kartNumber{};
    recordClock parsedClock{};

    for (auto [kartNumber, parsedClock] : records) {
        ++passNumberAndTimes[kartNumber - 1].first;

        if (passNumberAndTimes[kartNumber - 1].first > 1) // if the kart is not just starting
        {
            auto lapTime = parsedClock - passNumberAndTimes[kartNumber - 1].second;
            if (lapTime < bestLapTime)
            {
                bestLapTime = lapTime;
                bestLapKartNumber = kartNumber;
                bestLapNumber = passNumberAndTimes[kartNumber - 1].first - 1; // lap number is one less than pass number
            }
        }

        passNumberAndTimes[kartNumber - 1].second = parsedClock;

        // if one of karts finished the last lap don't take later records to account
        if (passNumberAndTimes[kartNumber - 1].first == passesCount)
            break;
    }

    return std::make_pair(bestLapKartNumber, bestLapNumber);
}

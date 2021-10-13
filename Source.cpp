#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <utility>
#include <string>
#include <vector>
#include <chrono>
#include <time.h>
#include <tuple>

using recordClock = std::chrono::system_clock::time_point;

struct timeRecord {
    unsigned int mKartNumber;
    recordClock mClockTime;

    timeRecord(unsigned int kartNumber, recordClock clockTime) :
        mKartNumber(kartNumber),
        mClockTime(clockTime) {
    }
};

class recordsReader {
public:
    ~recordsReader() {
        if (mRecordsStream.is_open()) {
            close();
        }
    }

    bool open(const std::string& filePath) {
        mRecordsStream.open(filePath);

        if (mRecordsStream.fail())
            return false;

        std::string unusedHeaders;
        std::getline(mRecordsStream, unusedHeaders);

        return true;
    }

    void close()
    {
        mRecordsStream.close();
    }

    std::vector<timeRecord> getRecords() {
        unsigned int kartNumber{};
        recordClock parsedClock{};

        std::vector<timeRecord> result;
        while (getNextRecord(kartNumber, parsedClock))
            result.emplace_back(kartNumber, parsedClock);

        return result;
    }
private:
    // returns true in case of successful read, returns false in case of finishing the string.
    // throws invalid argument exception when parse operation failes
    bool getNextRecord(unsigned int& currentKartNumber, recordClock& parsedClock) {
        std::string kartNumberString{};
        std::getline(mRecordsStream, kartNumberString, ',');

        if (mRecordsStream.fail())
            return false;

        currentKartNumber = std::stoul(kartNumberString);

        std::string currentClockString;
        std::getline(mRecordsStream, currentClockString);        
        parsedClock = parseClock(currentClockString);

        return true;
    }

    // Of course it is not hard to parse different parts of time expression and calculate the given time
    // But this way we take benefits of the standard methodes
    // and it makes it easier to support other time formats in future
    recordClock parseClock(const std::string& clockTime) const {
        std::stringstream timeStringStream(clockTime);
        std::time_t t = std::time(0);   // get time now
        std::tm parsedTimeObject;
        // Initialize the time struct with current date to have a complete date time after reading the time expressions
        localtime_s(&parsedTimeObject, &t);
        timeStringStream >> std::get_time(&parsedTimeObject, "%H:%M:%S");

        if (timeStringStream.fail())
        {
            throw std::invalid_argument("Can not parse the given timestamp: " + clockTime);
        }

        recordClock result = std::chrono::system_clock::from_time_t(std::mktime(&parsedTimeObject));

        return result;
    }

    std::ifstream mRecordsStream;
};

// This function returns the number of all karts participated in race as the first output
// And the maximum number of times an individual kart passed the start/finish line.
// This is equal to number of laps plus one
auto getNumberOfKartsAndPasses(const std::vector<timeRecord>& records) {
    std::vector<size_t> kartLastLaps{};
    size_t lapsCount{};

    for (const auto &record: records) {
        if (record.mKartNumber > kartLastLaps.size())
            kartLastLaps.resize(record.mKartNumber);

        ++kartLastLaps[record.mKartNumber - 1];

        if (kartLastLaps[record.mKartNumber - 1] > lapsCount)
            lapsCount = kartLastLaps[record.mKartNumber - 1];
    }

    return std::tuple{ kartLastLaps.size(), lapsCount };
}

auto getBestLap(recordsReader& reader)
{
    auto records = reader.getRecords();
    auto [kartsCount, passesCount] = getNumberOfKartsAndPasses(records);

    unsigned int bestLapKartNumber{};
    auto bestLapTime = std::chrono::system_clock::duration::max();
    size_t bestLapNumber{};

    // to keep it simple I assumed kart numbers always start from 1 and continues sequentially,
    // otherwise we can use a map instead of vector
    std::vector<std::pair<size_t, recordClock>> passNumberAndTimes(kartsCount); // pass number & its time for each kart
    unsigned int kartNumber{};
    recordClock parsedClock{};

    for(auto [kartNumber, parsedClock]: records) {
        ++passNumberAndTimes[kartNumber - 1].first;

        if (passNumberAndTimes[kartNumber - 1].first > 1) // if the kart is not just starting
        {
            auto lapTime = parsedClock - passNumberAndTimes[kartNumber - 1].second;
            if (lapTime < bestLapTime)
            {
                bestLapTime = lapTime;
                bestLapKartNumber = kartNumber;
                bestLapNumber = passNumberAndTimes[kartNumber - 1].first - 1;
            }
        }

        passNumberAndTimes[kartNumber - 1].second = parsedClock;

        // if one of karts finished the last lap don't take later records to account
        if (passNumberAndTimes[kartNumber - 1].first == passesCount)
            break;
    }

    return std::tuple{ bestLapKartNumber, bestLapTime };
}

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

    try {
        auto [bestLapKartNumber, bestLapTime] = getBestLap(reader);

        std::cout << "Best lap seconds: " << std::chrono::duration_cast<std::chrono::seconds>(bestLapTime).count() << std::endl;
        std::cout << "Best lap kart number : " << bestLapKartNumber << std::endl;
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
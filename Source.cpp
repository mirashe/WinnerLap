#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <chrono>
#include <time.h>
#include <tuple>

using recordClock = std::chrono::system_clock::time_point;

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
    }

    void close()
    {
        mRecordsStream.close();
    }

    // returns true in case of successful read, returns false in case of finishing the string.
    // throws domain exception when parse operation failes
    bool getNextRecord(unsigned int& currentKartNumber, recordClock& parsedClock) {
        std::string kartNumberString{};
        std::getline(mRecordsStream, kartNumberString, ',');

        if (mRecordsStream.fail())
            return false;

        currentKartNumber = std::stoul(kartNumberString);

        std::string currentClockString;
        std::getline(mRecordsStream, currentClockString);        
        parsedClock = parseClock(currentClockString);
    }

private:
    recordClock parseClock(const std::string& clockTime) const {
        std::stringstream timeStringStream(clockTime);
        std::time_t t = std::time(0);   // get time now
        std::tm parsedTimeObject;
        localtime_s(&parsedTimeObject, &t);
        timeStringStream >> std::get_time(&parsedTimeObject, "%H:%M:%S");

        if (timeStringStream.fail())
        {
            std::cerr << "parse failed";
            throw std::domain_error("Can not parse the given timestamp: " + clockTime);
        }

        auto tt = std::mktime(&parsedTimeObject);
        recordClock result = std::chrono::system_clock::from_time_t(tt);

        return result;
    }

    std::ifstream mRecordsStream;
};

auto getBestLap(recordsReader& reader)
{
    unsigned int bestLapKartNumber{};
    auto bestLapTime = std::chrono::system_clock::duration::max();

    // to keep it simple I assumed at the begining we always have all karts records ordered,
    // otherwise we can use a map instead of vector
    std::vector<recordClock> lapTimes;
    unsigned int kartNumber{};
    recordClock parsedClock{};

    while (reader.getNextRecord(kartNumber, parsedClock)) {
        if (kartNumber > lapTimes.size())
            lapTimes.push_back(parsedClock);
        else
        {
            auto lapTime = parsedClock - lapTimes[kartNumber - 1];
            if (lapTime.count() < bestLapTime.count())
            {
                bestLapTime = lapTime;
                bestLapKartNumber = kartNumber;
            }

            lapTimes[kartNumber - 1] = parsedClock;
        }
    }

    return std::tuple{ bestLapKartNumber, bestLapTime };
}

int main() {
    recordsReader reader;
    
    if (!reader.open("../karttimes.csv"))
        return 1;

    try {
        auto [bestLapKartNumber, bestLapTime] = getBestLap(reader);

        std::cout << "Best lap seconds: " << std::chrono::duration_cast<std::chrono::seconds>(bestLapTime).count() << std::endl;
        std::cout << "Best lap kart number : " << bestLapKartNumber << std::endl;
    }
    catch (std::exception ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
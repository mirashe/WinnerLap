#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <chrono>
#include <time.h>

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

    bool getNextRecord(unsigned int& currentKartNumber, recordClock& parsedClock) {
        std::string kartNumberString{};
        std::getline(mRecordsStream, kartNumberString, ',');

        if (mRecordsStream.fail())
            return false;

        currentKartNumber = std::stoul(kartNumberString);

        std::string currentClockString;
        std::getline(mRecordsStream, currentClockString);
        std::stringstream timeStringStream(currentClockString);

        std::time_t t = std::time(0);   // get time now
        std::tm now;
        localtime_s(&now, &t);

        std::tm parsedTimeObject{};
        timeStringStream >> std::get_time(&now, "%H:%M:%S");
        if (timeStringStream.fail())
        {
            std::cout << "parse failed";
            throw std::domain_error("Can not parse the given timestamp: " + currentClockString);
        }

        auto tt = std::mktime(&now);
        parsedClock = std::chrono::system_clock::from_time_t(tt);
    }

private:
    std::ifstream mRecordsStream;
};

int main() {
    recordsReader reader;
    
    if (!reader.open("../karttimes.csv"))
        return 1;

    // to keep it simple I assumed at the begining we always have all karts records ordered,
    // otherwise we can use a map instead of vector
    std::vector<recordClock> lapTimes;

    int bestLapKartNumber{};
    auto bestLapTime = std::chrono::system_clock::duration::max();

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
    
    std::cout << "Best lap seconds: " << std::chrono::duration_cast<std::chrono::seconds>(bestLapTime).count() << std::endl;
    std::cout << "Best lap kart number : " << bestLapKartNumber << std::endl;

    return 0;
}
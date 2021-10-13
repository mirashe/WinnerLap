#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <chrono>

using recordClock = std::chrono::system_clock::time_point;

struct timeRecord {
    size_t mKartNumber;
    recordClock mClockTime;

    timeRecord(size_t kartNumber, const recordClock& clockTime);
};

class recordsReader {
public:
    ~recordsReader();

    bool open(const std::string& filePath);

    void close();

    std::vector<timeRecord> getRecords();
private:
    // returns true in case of successful read, returns false in case of finishing the string.
    // throws invalid argument exception when parse operation failes
    bool getNextRecord(size_t& currentKartNumber, recordClock& parsedClock);

    // Of course it is not hard to parse different parts of time expression and calculate the given time
    // But this way we take benefits of the standard methodes
    // and it makes it easier to support other time formats in future
    recordClock parseClock(const std::string& clockTime) const;

    std::ifstream mRecordsStream;
};


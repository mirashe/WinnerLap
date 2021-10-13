#include "recordsReader.h"

#include <ctime>
#include <iomanip>
#include <sstream>

timeRecord::timeRecord(size_t kartNumber, const recordClock& clockTime) :
    mKartNumber(kartNumber),
    mClockTime(clockTime) {
}

recordsReader::~recordsReader() {
    if (mRecordsStream.is_open()) {
        close();
    }
}

bool recordsReader::open(const std::string& filePath) {
    mRecordsStream.open(filePath);

    if (mRecordsStream.fail())
        return false;

    std::string unusedHeaders;
    std::getline(mRecordsStream, unusedHeaders);

    return true;
}

void recordsReader::close() {
    mRecordsStream.close();
}

std::vector<timeRecord> recordsReader::getRecords() {
    size_t kartNumber{};
    recordClock parsedClock{};

    std::vector<timeRecord> result;
    while (getNextRecord(kartNumber, parsedClock))
        result.emplace_back(kartNumber, parsedClock);

    return result;
}

bool recordsReader::getNextRecord(size_t& currentKartNumber, recordClock& parsedClock) {
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

recordClock recordsReader::parseClock(const std::string& clockTime) const {
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

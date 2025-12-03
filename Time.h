/*#pragma once
#include <iostream>
#include <string>
#include <sstream>

class Time {
private:
    int hours;
    int minutes;

public:
    Time(int h = 0, int m = 0);
    bool parseFromString(const std::string& timeStr);
    bool isValid() const;
    bool operator<(const Time& other) const;
    bool operator<=(const Time& other) const;
    Time operator+(int minutesToAdd) const;
    friend std::ostream& operator<<(std::ostream& os, const Time& time);
};*/
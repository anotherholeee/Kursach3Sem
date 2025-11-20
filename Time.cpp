#include "Time.h"

Time::Time(int h, int m) : hours(h), minutes(m) {}

bool Time::parseFromString(const std::string& timeStr) {
    char colon;
    std::stringstream ss(timeStr);
    if (!(ss >> hours >> colon >> minutes) || colon != ':') {
        std::cout << "Ошибка: Неверный формат времени: " << timeStr << std::endl;
        return false;
    }
    if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59) {
        std::cout << "Ошибка: Некорректное время: " << timeStr << std::endl;
        return false;
    }
    return true;
}

bool Time::isValid() const {
    return hours >= 0 && hours <= 23 && minutes >= 0 && minutes <= 59;
}

bool Time::operator<(const Time& other) const {
    return (hours * 60 + minutes) < (other.hours * 60 + other.minutes);
}

bool Time::operator<=(const Time& other) const {
    return (hours * 60 + minutes) <= (other.hours * 60 + other.minutes);
}

Time Time::operator+(int minutesToAdd) const {
    int total = hours * 60 + minutes + minutesToAdd;
    return Time(total / 60, total % 60);
}

std::ostream& operator<<(std::ostream& os, const Time& time) {
    os << (time.hours < 10 ? "0" : "") << time.hours << ":"
       << (time.minutes < 10 ? "0" : "") << time.minutes;
    return os;
}
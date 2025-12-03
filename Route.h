/*#pragma once
#include <string>
#include <list>
#include <memory>
#include <iostream>

class Route {
private:
    int number;
    std::string vehicleType;
    std::list<std::string> stops;

public:
    Route(int num, const std::string& vType, const std::list<std::string>& stopsList);
    bool isValid() const;
    bool containsStop(const std::string& stop) const;
    int getStopPosition(const std::string& stop) const;
    bool isStopBefore(const std::string& stopA, const std::string& stopB) const;
    int getNumber() const;
    std::string getVehicleType() const;
    std::string getStartStop() const;
    std::string getEndStop() const;
    const std::list<std::string>& getStops() const;
};*/
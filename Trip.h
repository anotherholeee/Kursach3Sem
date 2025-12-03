/*#pragma once
#include <memory>
#include <map>
#include "Route.h"
#include "Time.h"

class Trip {
private:
    int tripId;
    std::shared_ptr<Route> route;
    Time startTime;
    std::map<std::string, Time> schedule;

public:
    Trip(int id, std::shared_ptr<Route> r, const Time& start);
    bool generateSchedule();
    bool getArrivalTime(const std::string& stop, Time& result) const;
    bool hasStop(const std::string& stop) const;
    int getTripId() const;
    std::shared_ptr<Route> getRoute() const;
    Time getStartTime() const;
};*/
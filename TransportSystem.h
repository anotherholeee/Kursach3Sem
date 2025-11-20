#pragma once
#include <map>
#include <list>
#include <memory>
#include "Route.h"
#include "Trip.h"

class TransportSystem {
private:
    std::map<int, std::shared_ptr<Route>> routes;
    std::map<int, std::shared_ptr<Trip>> trips;

public:
    std::list<std::shared_ptr<Route>> findRoutes(const std::string& from, const std::string& to);
    void showStopSchedule(const std::string& stopName, const Time& from, const Time& to);
    bool addRoute(std::shared_ptr<Route> route);
    bool addTrip(std::shared_ptr<Trip> trip);
    bool removeRoute(int number);
    std::list<std::shared_ptr<Route>> findRoutesThroughStop(const std::string& stop);
    std::shared_ptr<Route> findRouteByNumber(int number);
    std::list<std::shared_ptr<Route>> getAllRoutesList() const;
    const std::map<int, std::shared_ptr<Route>>& getRoutes() const;
    const std::map<int, std::shared_ptr<Trip>>& getTrips() const;
};
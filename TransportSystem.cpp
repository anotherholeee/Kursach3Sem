#include "TransportSystem.h"

std::list<std::shared_ptr<Route>> TransportSystem::findRoutes(const std::string& from, const std::string& to) {
    std::list<std::shared_ptr<Route>> result;
    
    if (from.empty() || to.empty()) {
        std::cout << "Ошибка: Названия остановок не могут быть пустыми" << std::endl;
        return result;
    }

    for (const auto& [number, route] : routes) {
        if (route->containsStop(from) && route->containsStop(to) &&
            route->isStopBefore(from, to)) {
            result.push_back(route);
        }
    }
    return result;
}

void TransportSystem::showStopSchedule(const std::string& stopName, const Time& from, const Time& to) {
    if (stopName.empty()) {
        std::cout << "Ошибка: Название остановки не может быть пустым" << std::endl;
        return;
    }

    if (!from.isValid() || !to.isValid()) {
        std::cout << "Ошибка: Некорректный временной диапазон" << std::endl;
        return;
    }

    std::multimap<Time, int> schedule;

    for (const auto& [tripId, trip] : trips) {
        if (trip->hasStop(stopName)) {
            Time arrival;
            if (trip->getArrivalTime(stopName, arrival)) {
                if (from <= arrival && arrival <= to) {
                    schedule.insert({arrival, trip->getRoute()->getNumber()});
                }
            }
        }
    }

    std::cout << "\nРасписание для '" << stopName << "' (" << from << " - " << to << "):\n";
    if (schedule.empty()) {
        std::cout << "Рейсов не найдено.\n";
    } else {
        for (const auto& [time, routeNumber] : schedule) {
            std::cout << "Маршрут " << routeNumber << " - " << time << '\n';
        }
    }
}

bool TransportSystem::addRoute(std::shared_ptr<Route> route) {
    if (!route) {
        std::cout << "Ошибка: Попытка добавить пустой маршрут" << std::endl;
        return false;
    }

    if (!route->isValid()) {
        return false;
    }

    int number = route->getNumber();
    if (routes.find(number) != routes.end()) {
        std::cout << "Ошибка: Маршрут с номером " << number << " уже существует" << std::endl;
        return false;
    }
    
    routes[number] = route;
    return true;
}

bool TransportSystem::addTrip(std::shared_ptr<Trip> trip) {
    if (!trip) {
        std::cout << "Ошибка: Попытка добавить пустой рейс" << std::endl;
        return false;
    }

    int tripId = trip->getTripId();
    if (trips.find(tripId) != trips.end()) {
        std::cout << "Ошибка: Рейс с ID " << tripId << " уже существует" << std::endl;
        return false;
    }

    if (!trip->generateSchedule()) {
        std::cout << "Ошибка: Не удалось сгенерировать расписание для рейса" << std::endl;
        return false;
    }
    
    trips[tripId] = trip;
    return true;
}

bool TransportSystem::removeRoute(int number) {
    auto it = routes.find(number);
    if (it != routes.end()) {
        // Удаляем связанные рейсы
        for (auto tripIt = trips.begin(); tripIt != trips.end(); ) {
            if (tripIt->second->getRoute()->getNumber() == number) {
                tripIt = trips.erase(tripIt);
            } else {
                ++tripIt;
            }
        }

        routes.erase(it);
        std::cout << "Маршрут " << number << " удален.\n";
        return true;
    }
    
    std::cout << "Ошибка: Маршрут с номером " << number << " не найден" << std::endl;
    return false;
}

std::list<std::shared_ptr<Route>> TransportSystem::findRoutesThroughStop(const std::string& stop) {
    std::list<std::shared_ptr<Route>> result;
    
    if (stop.empty()) {
        std::cout << "Ошибка: Название остановки не может быть пустым" << std::endl;
        return result;
    }

    for (const auto& [number, route] : routes) {
        if (route->containsStop(stop)) {
            result.push_back(route);
        }
    }
    return result;
}

std::shared_ptr<Route> TransportSystem::findRouteByNumber(int number) {
    auto it = routes.find(number);
    return (it != routes.end()) ? it->second : nullptr;
}

std::list<std::shared_ptr<Route>> TransportSystem::getAllRoutesList() const {
    std::list<std::shared_ptr<Route>> result;
    for (const auto& [number, route] : routes) {
        result.push_back(route);
    }
    return result;
}

const std::map<int, std::shared_ptr<Route>>& TransportSystem::getRoutes() const { return routes; }
const std::map<int, std::shared_ptr<Trip>>& TransportSystem::getTrips() const { return trips; }
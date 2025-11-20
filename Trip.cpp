#include "Trip.h"

Trip::Trip(int id, std::shared_ptr<Route> r, const Time& start)
    : tripId(id), route(r), startTime(start) {}

bool Trip::generateSchedule() {
    if (!route) {
        std::cout << "Ошибка: Отсутствует маршрут для рейса" << std::endl;
        return false;
    }
    
    const auto& stops = route->getStops();
    if (stops.empty()) {
        std::cout << "Ошибка: Маршрут пустой" << std::endl;
        return false;
    }

    if (!startTime.isValid()) {
        std::cout << "Ошибка: Некорректное время начала рейса" << std::endl;
        return false;
    }

    Time currentTime = startTime;
    schedule.clear();

    // Первая остановка
    schedule[stops.front()] = currentTime;

    // Последующие остановки (2 минуты на перегон + 1 минута на остановке)
    auto it = stops.begin();
    ++it;
    for (; it != stops.end(); ++it) {
        currentTime = currentTime + 2; // время между остановками
        schedule[*it] = currentTime;
        currentTime = currentTime + 1; // время стоянки
    }
    
    return true;
}

bool Trip::getArrivalTime(const std::string& stop, Time& result) const {
    auto it = schedule.find(stop);
    if (it != schedule.end()) {
        result = it->second;
        return true;
    }
    std::cout << "Ошибка: Остановка '" << stop << "' не найдена в расписании" << std::endl;
    return false;
}

bool Trip::hasStop(const std::string& stop) const {
    return schedule.find(stop) != schedule.end();
}

int Trip::getTripId() const { return tripId; }
std::shared_ptr<Route> Trip::getRoute() const { return route; }
Time Trip::getStartTime() const { return startTime; }
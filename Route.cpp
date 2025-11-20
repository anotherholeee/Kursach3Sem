#include "Route.h"
#include <algorithm>

Route::Route(int num, const std::string& vType, const std::list<std::string>& stopsList)
    : number(num), vehicleType(vType), stops(stopsList) {}

bool Route::isValid() const {
    if (stops.empty()) {
        std::cout << "Ошибка: Маршрут не может быть пустым" << std::endl;
        return false;
    }
    return true;
}

bool Route::containsStop(const std::string& stop) const {
    return std::find(stops.begin(), stops.end(), stop) != stops.end();
}

int Route::getStopPosition(const std::string& stop) const {
    int pos = 0;
    for (const auto& s : stops) {
        if (s == stop) return pos;
        pos++;
    }
    return -1;
}

bool Route::isStopBefore(const std::string& stopA, const std::string& stopB) const {
    int posA = getStopPosition(stopA);
    int posB = getStopPosition(stopB);
    return posA != -1 && posB != -1 && posA < posB;
}

int Route::getNumber() const { return number; }
std::string Route::getVehicleType() const { return vehicleType; }
std::string Route::getStartStop() const { return stops.front(); }
std::string Route::getEndStop() const { return stops.back(); }
const std::list<std::string>& Route::getStops() const { return stops; }
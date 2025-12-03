#include <iostream>
#include <windows.h>
#include <utility>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <map>
#include <ctime>
#include <limits>
#include <unordered_map>
#include <set>
#include <queue>
#include <filesystem>
#include <iomanip>

class TransportSystem;

// Базовый класс для исключений транспортной системы
class TransportException : public std::exception {
private:
    std::string message;
public:
    explicit TransportException(std::string  msg) : message(std::move(msg)) {}
    const char* what() const noexcept override { return message.c_str(); }
};

class Stop {
private:
    int id;
    std::string name;
public:
    Stop(int stopId, std::string  stopName) : id(stopId), name(std::move(stopName)) {}

    int getId() const { return id; }
    std::string getName() const { return name; }

    bool operator==(const Stop& other) const {
        return id == other.id;
    }

    std::string serialize() const {
        return std::to_string(id) + "|" + name;
    }

    static Stop deserialize(const std::string& data) {
        std::istringstream ss(data);
        std::string idStr, name;
        std::getline(ss, idStr, '|');
        std::getline(ss, name);
        return Stop(std::stoi(idStr), name);
    }
};

// Класс для времени
class Time {
private:
    int hours;
    int minutes;

    // Вспомогательная функция для нормализации
    void normalize(int totalMinutes) {
        totalMinutes = ((totalMinutes % 1440) + 1440) % 1440; // [0, 1439]
        hours = totalMinutes / 60;
        minutes = totalMinutes % 60;
    }

public:
    explicit Time(int h = 0, int m = 0) {
        normalize(h * 60 + m);
    }

    explicit Time(const std::string& timeStr) {
        std::stringstream ss(timeStr);
        char colon;
        int h, m;
        if (!(ss >> h >> colon >> m) || colon != ':' || ss.fail() || !ss.eof()) {
            throw TransportException("Неверный формат времени: " + timeStr);
        }
        if (h < 0 || h > 23 || m < 0 || m > 59) {
            throw TransportException("Некорректное время: " + timeStr);
        }
        hours = h;
        minutes = m;
    }

    int getTotalMinutes() const { return hours * 60 + minutes; }
    int getHours() const { return hours; }
    int getMinutes() const { return minutes; }

    bool operator<(const Time& other) const {
        return getTotalMinutes() < other.getTotalMinutes();
    }

    bool operator<=(const Time& other) const {
        return getTotalMinutes() <= other.getTotalMinutes();
    }

    bool operator>(const Time& other) const {
        return getTotalMinutes() > other.getTotalMinutes();
    }

    bool operator>=(const Time& other) const {
        return getTotalMinutes() >= other.getTotalMinutes();
    }

    bool operator==(const Time& other) const {
        return getTotalMinutes() == other.getTotalMinutes();
    }

    bool operator!=(const Time& other) const {
        return getTotalMinutes() != other.getTotalMinutes();
    }

    Time operator+(int minutesToAdd) const {
        return Time(0, getTotalMinutes() + minutesToAdd);
    }

    Time operator-(int minutesToSubtract) const {
        return Time(0, getTotalMinutes() - minutesToSubtract);
    }

    int operator-(const Time& other) const {
        return getTotalMinutes() - other.getTotalMinutes();
    }

    friend std::ostream& operator<<(std::ostream& os, const Time& time) {
        os << (time.hours < 10 ? "0" : "") << time.hours << ":"
           << (time.minutes < 10 ? "0" : "") << time.minutes;
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Time& time) {
        std::string timeStr;
        is >> timeStr;
        time = Time(timeStr);
        return is;
    }

    std::string serialize() const {
        return (hours < 10 ? "0" : "") + std::to_string(hours) + ":" +
               (minutes < 10 ? "0" : "") + std::to_string(minutes);
    }

    static Time deserialize(const std::string& data) {
        return Time(data);
    }
};

// Вперед объявления для транспортных средств
class Bus;
class Tram;
class Trolleybus;

// Базовый класс для транспортного средства
class Vehicle {
protected:
    std::string type;
    std::string model;
    std::string licensePlate;
public:
    Vehicle(const std::string& t, const std::string& m, const std::string& lp)
        : type(t), model(m), licensePlate(lp) {}

    virtual ~Vehicle() = default;

    std::string getInfo() const {
        return type + " " + model + " " + licensePlate;
    }

    std::string getType() const { return type; }
    std::string getModel() const { return model; }
    std::string getLicensePlate() const { return licensePlate; }

    virtual std::string serialize() const {
        return type + "|" + model + "|" + licensePlate;
    }

    static std::shared_ptr<Vehicle> deserialize(const std::string& data) {
        std::istringstream ss(data);
        std::string type, model, licensePlate;
        std::getline(ss, type, '|');
        std::getline(ss, model, '|');
        std::getline(ss, licensePlate);

        // Используем фабричный метод для создания объектов
        // Создание объектов будет выполнено в DataManager
        throw TransportException("Используйте DataManager для создания транспортных средств");
    }
};

// Классы конкретных транспортных средств
class Bus : public Vehicle {
public:
    Bus(const std::string& m, const std::string& lp)
        : Vehicle("Автобус", m, lp) {}
};

class Tram : public Vehicle {
public:
    Tram(const std::string& m, const std::string& lp)
        : Vehicle("Трамвай", m, lp) {}
};

class Trolleybus : public Vehicle {
public:
    Trolleybus(const std::string& m, const std::string& lp)
        : Vehicle("Троллейбус", m, lp) {}
};

// Класс водителя
class Driver {
private:
    std::string firstName;
    std::string lastName;
    std::string middleName;
public:
    Driver(const std::string& fname, const std::string& lname, const std::string& mname = "")
        : firstName(fname), lastName(lname), middleName(mname) {}

    std::string getFullName() const {
        return lastName + " " + firstName + (middleName.empty() ? "" : " " + middleName);
    }

    std::string getFirstName() const { return firstName; }
    std::string getLastName() const { return lastName; }
    std::string getMiddleName() const { return middleName; }

    bool operator==(const Driver& other) const {
        return firstName == other.firstName &&
               lastName == other.lastName &&
               middleName == other.middleName;
    }

    std::string serialize() const {
        return firstName + "|" + lastName + "|" + middleName;
    }

    static std::shared_ptr<Driver> deserialize(const std::string& data) {
        std::istringstream ss(data);
        std::string firstName, lastName, middleName;
        std::getline(ss, firstName, '|');
        std::getline(ss, lastName, '|');
        std::getline(ss, middleName);
        return std::make_shared<Driver>(firstName, lastName, middleName);
    }
};

class Route {
private:
    int number;
    std::string vehicleType;
    std::string startStop;
    std::string endStop;
    std::vector<std::string> allStops;

public:
    Route(int num, const std::string& vType, const std::vector<std::string>& stops)
        : number(num), vehicleType(vType), allStops(stops) {
        if (stops.empty()) {
            throw TransportException("Маршрут не может быть пустым");
        }
        startStop = stops.front();
        endStop = stops.back();
    }

    bool containsStop(const std::string& stop) const {
        return std::find(allStops.begin(), allStops.end(), stop) != allStops.end();
    }

    int getStopPosition(const std::string& stop) const {
        auto it = std::find(allStops.begin(), allStops.end(), stop);
        if (it != allStops.end()) {
            return static_cast<int>(std::distance(allStops.begin(), it));
        }
        return -1;
    }

    bool isStopBefore(const std::string& stopA, const std::string& stopB) const {
        int posA = getStopPosition(stopA);
        int posB = getStopPosition(stopB);
        return posA != -1 && posB != -1 && posA < posB;
    }

    int getNumber() const { return number; }
    std::string getVehicleType() const { return vehicleType; }
    std::string getStartStop() const { return startStop; }
    std::string getEndStop() const { return endStop; }
    const std::vector<std::string>& getAllStops() const { return allStops; }

    std::string serialize() const {
        std::string result = std::to_string(number) + "|" + vehicleType + "|";
        for (size_t i = 0; i < allStops.size(); ++i) {
            result += allStops[i];
            if (i < allStops.size() - 1) result += ";";
        }
        return result;
    }

    static std::shared_ptr<Route> deserialize(const std::string& data) {
        std::istringstream ss(data);
        std::string numberStr, vehicleType, stopsStr;
        std::getline(ss, numberStr, '|');
        std::getline(ss, vehicleType, '|');
        std::getline(ss, stopsStr);

        std::vector<std::string> stops;
        std::istringstream stopsStream(stopsStr);
        std::string stop;
        while (std::getline(stopsStream, stop, ';')) {
            stops.push_back(stop);
        }

        return std::make_shared<Route>(std::stoi(numberStr), vehicleType, stops);
    }
};

// Вперед объявление для TransportSystem
class TransportSystem;

// Класс рейса
class Trip {
private:
    int tripId;
    std::shared_ptr<Route> route;
    std::shared_ptr<Vehicle> vehicle;
    std::shared_ptr<Driver> driver;
    Time startTime;
    std::map<std::string, Time> schedule; // остановка -> время прибытия

public:
    Trip(int id, std::shared_ptr<Route> r, std::shared_ptr<Vehicle> v,
         std::shared_ptr<Driver> d, const Time& start)
        : tripId(id), route(std::move(r)), vehicle(std::move(v)),
          driver(std::move(d)), startTime(start) {}

    void setArrivalTime(const std::string& stop, const Time& time) {
        schedule[stop] = time;
    }

    Time getArrivalTime(const std::string& stop) const {
        auto it = schedule.find(stop);
        if (it != schedule.end()) {
            return it->second;
        }
        throw TransportException("Остановка не найдена в расписании рейса");
    }

    bool hasStop(const std::string& stop) const {
        return schedule.find(stop) != schedule.end();
    }

    int getTripId() const { return tripId; }
    std::shared_ptr<Route> getRoute() const { return route; }
    std::shared_ptr<Vehicle> getVehicle() const { return vehicle; }
    std::shared_ptr<Driver> getDriver() const { return driver; }
    Time getStartTime() const { return startTime; }
    const std::map<std::string, Time>& getSchedule() const { return schedule; }

    Time getEstimatedEndTime() const {
        return startTime + 60; // Предполагаем 1 час для поездки
    }

    std::string serialize() const {
        std::string result = std::to_string(tripId) + "|" + route->serialize() + "|" +
                           vehicle->serialize() + "|" + driver->serialize() + "|" +
                           startTime.serialize() + "|";

        // Сериализация расписания
        std::string scheduleStr;
        for (const auto& [stop, time] : schedule) {
            scheduleStr += stop + "=" + time.serialize() + ";";
        }
        if (!scheduleStr.empty()) scheduleStr.pop_back(); // Удаляем последнюю точку с запятой
        result += scheduleStr;

        return result;
    }

    static std::shared_ptr<Trip> deserialize(const std::string& data, TransportSystem* system = nullptr);
};

// Класс для хранения информации о поездке с пересадками
class Journey {
private:
    std::vector<std::shared_ptr<Trip>> trips; // Рейсы, составляющие поездку
    std::vector<std::string> transferPoints; // Точки пересадок
    Time startTime;
    Time endTime;
    int transferCount;

public:
    Journey(const std::vector<std::shared_ptr<Trip>>& tripList,
            const std::vector<std::string>& transfers,
            Time start, Time end)
        : trips(tripList), transferPoints(transfers),
          startTime(start), endTime(end),
          transferCount(static_cast<int>(transfers.size())) {}

    int getTotalDuration() const {
        return endTime - startTime;
    }

    int getTransferCount() const { return transferCount; }
    Time getStartTime() const { return startTime; }
    Time getEndTime() const { return endTime; }
    const std::vector<std::shared_ptr<Trip>>& getTrips() const { return trips; }
    const std::vector<std::string>& getTransferPoints() const { return transferPoints; }

    void display() const {
        std::cout << "\nМаршрут поездки:\n";
        std::cout << "Общее время: " << getTotalDuration() << " минут\n";
        std::cout << "Пересадок: " << transferCount << "\n";
        std::cout << "Время отправления: " << startTime << "\n";
        std::cout << "Время прибытия: " << endTime << "\n";

        for (size_t i = 0; i < trips.size(); ++i) {
            std::cout << "\nЭтап " << (i + 1) << ":\n";
            std::cout << "  Маршрут: " << trips[i]->getRoute()->getNumber() << "\n";
            std::cout << "  Транспорт: " << trips[i]->getVehicle()->getInfo() << "\n";

            if (i > 0) {
                std::cout << "  Пересадка на: " << transferPoints[i - 1] << "\n";
            }
        }
    }
};

// Класс планировщика поездок
class JourneyPlanner {
private:
    TransportSystem* system;

public:
    JourneyPlanner(TransportSystem* sys) : system(sys) {}

    std::vector<Journey> findJourneysWithTransfers(const std::string& startStop,
                                                   const std::string& endStop,
                                                   const Time& departureTime,
                                                   int maxTransfers = 2) const;

    Journey findFastestJourney(const std::string& startStop,
                               const std::string& endStop,
                               const Time& departureTime);

    Journey findJourneyWithLeastTransfers(const std::string& startStop,
                                          const std::string& endStop,
                                          const Time& departureTime);

    void displayJourney(const Journey& journey) const {
        journey.display();
    }
};

// Класс для управления графиком водителей
class DriverSchedule {
private:
    std::unordered_map<std::shared_ptr<Driver>,
                       std::vector<std::shared_ptr<Trip>>> driverTrips;
    const int MAX_WORKING_HOURS = 12 * 60; // 12 часов в минутах

public:
    void assignTripToDriver(std::shared_ptr<Driver> driver,
                           std::shared_ptr<Trip> trip) {
        driverTrips[driver].push_back(trip);
    }

    void removeTripFromDriver(std::shared_ptr<Driver> driver, int tripId) {
        auto& trips = driverTrips[driver];
        trips.erase(std::remove_if(trips.begin(), trips.end(),
            [tripId](const auto& trip) {
                return trip->getTripId() == tripId;
            }), trips.end());
    }

    bool isDriverAvailable(std::shared_ptr<Driver> driver,
                          const Time& startTime,
                          const Time& endTime) const {
        auto it = driverTrips.find(driver);
        if (it == driverTrips.end()) return true;

        for (const auto& trip : it->second) {
            Time tripStart = trip->getStartTime();
            Time tripEnd = tripStart + 60; // Предполагаем 1 час для поездки

            if (!(endTime < tripStart || startTime > tripEnd)) {
                return false; // Найдено пересечение
            }
        }
        return true;
    }

    bool checkWorkingHoursCompliance(std::shared_ptr<Driver> driver) const {
        auto it = driverTrips.find(driver);
        if (it == driverTrips.end()) return true;

        int totalMinutes = 0;
        for (const auto& trip : it->second) {
            totalMinutes += 60; // Предполагаем 1 час на поездку
        }

        return totalMinutes <= MAX_WORKING_HOURS;
    }

    std::vector<std::shared_ptr<Trip>> getDriverTrips(
        std::shared_ptr<Driver> driver) const {
        auto it = driverTrips.find(driver);
        if (it != driverTrips.end()) {
            return it->second;
        }
        return {};
    }

    int getTotalWorkingMinutes(std::shared_ptr<Driver> driver) const {
        auto trips = getDriverTrips(driver);
        return static_cast<int>(trips.size()) * 60;
    }
};

// Класс для управления данными (сохранение/загрузка в текстовые файлы)
class DataManager {
private:
    std::string dataDirectory;

public:
    DataManager(const std::string& dir = "data/") : dataDirectory(dir) {
        // Создаем директорию, если её нет
        std::filesystem::create_directories(dataDirectory);
    }

    // Сохранение всех данных в текстовые файлы
    void saveAllData(TransportSystem& system);

    // Загрузка всех данных из текстовых файлов
    void loadAllData(TransportSystem& system);

private:
    void saveStops(TransportSystem& system);
    void saveVehicles(TransportSystem& system);
    void saveDrivers(TransportSystem& system);
    void saveRoutes(TransportSystem& system);
    void saveTrips(TransportSystem& system);
    void saveAdminCredentials(TransportSystem& system);

    void loadStops(TransportSystem& system);
    void loadVehicles(TransportSystem& system);
    void loadDrivers(TransportSystem& system);
    void loadRoutes(TransportSystem& system);
    void loadTrips(TransportSystem& system);
    void loadAdminCredentials(TransportSystem& system);
};

// Класс транспортной системы
class TransportSystem {
private:
    std::vector<std::shared_ptr<Route>> routes;
    std::vector<std::shared_ptr<Trip>> trips;
    std::vector<std::shared_ptr<Vehicle>> vehicles;
    std::vector<std::shared_ptr<Driver>> drivers;
    std::vector<Stop> stops;
    std::unordered_map<int, std::string> stopIdToName;
    std::unordered_map<std::string, std::string> adminCredentials;

    // Новые компоненты
    JourneyPlanner journeyPlanner;
    DriverSchedule driverSchedule;
    DataManager dataManager;

public:
    TransportSystem() : journeyPlanner(this), dataManager() {
        // Инициализация учетных данных администраторов
        adminCredentials["admin"] = "admin123";
        adminCredentials["manager"] = "manager123";
    }

    // Аутентификация администратора
    bool authenticateAdmin(const std::string& username, const std::string& password) {
        auto it = adminCredentials.find(username);
        return it != adminCredentials.end() && it->second == password;
    }

    // Добавление нового администратора
    void addAdmin(const std::string& username, const std::string& password) {
        adminCredentials[username] = password;
    }

    // Сохранение данных
    void saveData() {
        dataManager.saveAllData(*this);
    }

    // Загрузка данных
    void loadData() {
        dataManager.loadAllData(*this);
    }

    // Функция поиска маршрутов между двумя остановками
    std::vector<std::shared_ptr<Route>> findRoutes(const std::string& stopA, const std::string& stopB) {
        std::vector<std::shared_ptr<Route>> foundRoutes;
        for (const auto& route : routes) {
            if (route->containsStop(stopA) &&
                route->containsStop(stopB) &&
                route->isStopBefore(stopA, stopB)) {
                foundRoutes.push_back(route);
            }
        }
        return foundRoutes;
    }

    // Просмотр расписания для остановки
    void getStopTimetable(int stopId, const Time& startTime, const Time& endTime) {
        auto it = stopIdToName.find(stopId);
        if (it == stopIdToName.end()) {
            throw TransportException("Остановка с ID " + std::to_string(stopId) + " не найдена");
        }
        const std::string& stopName = it->second;

        std::vector<std::pair<int, Time>> relevantTrips;

        for (const auto& trip : trips) {
            if (trip->hasStop(stopName)) {
                Time arrivalTime = trip->getArrivalTime(stopName);
                if (startTime <= arrivalTime && arrivalTime <= endTime) {
                    relevantTrips.push_back({trip->getRoute()->getNumber(), arrivalTime});
                }
            }
        }

        std::sort(relevantTrips.begin(), relevantTrips.end(),
                  [](const auto& a, const auto& b) { return a.second < b.second; });

        std::cout << "\nРасписание для остановки '" << stopName << "' с "
                  << startTime << " по " << endTime << ":\n";
        if (relevantTrips.empty()) {
            std::cout << "Рейсов не найдено.\n";
        } else {
            for (const auto& trip : relevantTrips) {
                std::cout << "Маршрут " << trip.first << " - прибытие в " << trip.second << '\n';
            }
        }
    }

    // Расчет времени прибытия для рейса - ИСПРАВЛЕННАЯ ВЕРСИЯ
    void calculateArrivalTimes(int tripId, double averageSpeed) {
        if (averageSpeed <= 0) {
            throw TransportException("Средняя скорость должна быть положительной");
        }

        auto tripIt = std::find_if(trips.begin(), trips.end(),
                                   [tripId](const auto& t) { return t->getTripId() == tripId; });

        if (tripIt == trips.end()) {
            throw TransportException("Рейс с ID " + std::to_string(tripId) + " не найден");
        }

        auto trip = *tripIt;
        const auto& stopsList = trip->getRoute()->getAllStops();

        if (stopsList.empty()) {
            throw TransportException("Маршрут не содержит остановок");
        }

        Time currentTime = trip->getStartTime();
        trip->setArrivalTime(stopsList[0], currentTime);

        const double distanceBetweenStops = 1.5; // км
        const int stopTime = 1; // минута

        for (size_t i = 1; i < stopsList.size(); ++i) {
            double travelTimeMinutes = (distanceBetweenStops / averageSpeed) * 60;
            Time arrivalTime = currentTime + static_cast<int>(travelTimeMinutes + 0.5);
            trip->setArrivalTime(stopsList[i], arrivalTime);
            currentTime = arrivalTime + stopTime;
        }

        std::cout << "Расписание для рейса " << tripId << " рассчитано.\n";
    }

    // АДМИНИСТРАТИВНЫЕ ФУНКЦИИ
    void addRoute(std::shared_ptr<Route> route) {
        // Проверка на уникальность номера маршрута
        for (const auto& existingRoute : routes) {
            if (existingRoute->getNumber() == route->getNumber()) {
                throw TransportException("Маршрут с номером " + std::to_string(route->getNumber()) + " уже существует");
            }
        }
        routes.push_back(std::move(route));
    }

    void addTrip(std::shared_ptr<Trip> trip) {
        // Проверка на уникальность ID рейса
        for (const auto& existingTrip : trips) {
            if (existingTrip->getTripId() == trip->getTripId()) {
                throw TransportException("Рейс с ID " + std::to_string(trip->getTripId()) + " уже существует");
            }
        }

        // Проверка, что водитель существует
        bool driverExists = std::find(drivers.begin(), drivers.end(),
                                      trip->getDriver()) != drivers.end();
        if (!driverExists) {
            throw TransportException("Водитель не зарегистрирован в системе!");
        }

        // Проверка, что транспорт существует
        bool vehicleExists = std::find(vehicles.begin(), vehicles.end(),
                                       trip->getVehicle()) != vehicles.end();
        if (!vehicleExists) {
            throw TransportException("Транспорт не зарегистрирован в системе!");
        }

        trips.push_back(std::move(trip));
    }

    void addVehicle(std::shared_ptr<Vehicle> vehicle) {
        // Проверка на уникальность номерного знака
        for (const auto& existingVehicle : vehicles) {
            if (existingVehicle->getLicensePlate() == vehicle->getLicensePlate()) {
                throw TransportException("Транспортное средство с номером " + vehicle->getLicensePlate() + " уже существует");
            }
        }
        vehicles.push_back(std::move(vehicle));
    }

    void addDriver(std::shared_ptr<Driver> driver) {
        drivers.push_back(std::move(driver));
    }

    void addStop(const Stop& stop) {
        // Проверка на уникальность ID остановки
        for (const auto& existingStop : stops) {
            if (existingStop.getId() == stop.getId()) {
                throw TransportException("Остановка с ID " + std::to_string(stop.getId()) + " уже существует");
            }
        }
        stops.push_back(stop);
        stopIdToName[stop.getId()] = stop.getName();
    }

    void removeRoute(int routeNumber) {
        auto it = std::find_if(routes.begin(), routes.end(),
                              [routeNumber](const auto& r) { return r->getNumber() == routeNumber; });
        if (it != routes.end()) {
            routes.erase(it);
            std::cout << "Маршрут " << routeNumber << " удален.\n";
        } else {
            throw TransportException("Маршрут с номером " + std::to_string(routeNumber) + " не найден");
        }
    }

    void removeTrip(int tripId) {
        auto it = std::find_if(trips.begin(), trips.end(),
                              [tripId](const auto& t) { return t->getTripId() == tripId; });
        if (it != trips.end()) {
            trips.erase(it);
            std::cout << "Рейс " << tripId << " удален.\n";
        } else {
            throw TransportException("Рейс с ID " + std::to_string(tripId) + " не найден");
        }
    }

    // Просмотр всех данных
    void displayAllRoutes() const {
        std::cout << "\n=== ВСЕ МАРШРУТЫ ===\n";
        for (const auto& route : routes) {
            std::cout << "Маршрут " << route->getNumber() << " (" << route->getVehicleType()
                      << "): " << route->getStartStop() << " -> " << route->getEndStop() << "\n";
        }
    }

    void displayAllTrips() const {
        std::cout << "\n=== ВСЕ РЕЙСЫ ===\n";
        for (const auto& trip : trips) {
            std::cout << "Рейс " << trip->getTripId() << ": Маршрут " << trip->getRoute()->getNumber()
                      << ", ТС: " << trip->getVehicle()->getInfo()
                      << ", Водитель: " << trip->getDriver()->getFullName()
                      << ", Отправление: " << trip->getStartTime() << "\n";
        }
    }

    void displayAllVehicles() const {
        std::cout << "\n=== ВСЕ ТРАНСПОРТНЫЕ СРЕДСТВА ===\n";
        for (const auto& vehicle : vehicles) {
            std::cout << vehicle->getInfo() << '\n';
        }
    }

    void displayAllStops() const {
        std::cout << "\n=== ВСЕ ОСТАНОВКИ ===\n";
        for (const auto& stop : stops) {
            std::cout << "ID: " << stop.getId() << " - " << stop.getName() << '\n';
        }
    }

    // Получение всех рейсов
    const std::vector<std::shared_ptr<Trip>>& getTrips() const { return trips; }
    const std::vector<std::shared_ptr<Route>>& getRoutes() const { return routes; }
    const std::vector<std::shared_ptr<Vehicle>>& getVehicles() const { return vehicles; }
    const std::vector<Stop>& getStops() const { return stops; }
    const std::vector<std::shared_ptr<Driver>>& getDrivers() const { return drivers; }

    // Получение компонентов
    JourneyPlanner& getJourneyPlanner() { return journeyPlanner; }
    DriverSchedule& getDriverSchedule() { return driverSchedule; }

    // Поиск водителя по ФИО
    std::shared_ptr<Driver> findDriverByName(const std::string& firstName,
                                            const std::string& lastName,
                                            const std::string& middleName = "") const {
        for (const auto& driver : drivers) {
            if (driver->getFirstName() == firstName &&
                driver->getLastName() == lastName &&
                (middleName.empty() || driver->getMiddleName() == middleName)) {
                return driver;
            }
        }
        return nullptr;
    }

    // Поиск транспорта по номеру
    std::shared_ptr<Vehicle> findVehicleByLicensePlate(const std::string& licensePlate) const {
        for (const auto& vehicle : vehicles) {
            if (vehicle->getLicensePlate() == licensePlate) {
                return vehicle;
            }
        }
        return nullptr;
    }

    // Поиск маршрута по номеру
    std::shared_ptr<Route> findRouteByNumber(int number) const {
        for (const auto& route : routes) {
            if (route->getNumber() == number) {
                return route;
            }
        }
        return nullptr;
    }

    // Получение всех рейсов через остановку
    std::vector<std::shared_ptr<Trip>> getTripsThroughStop(const std::string& stopName) const {
        std::vector<std::shared_ptr<Trip>> result;
        for (const auto& trip : trips) {
            if (trip->hasStop(stopName)) {
                result.push_back(trip);
            }
        }
        return result;
    }

    // Получение остановки по ID
    std::string getStopNameById(int id) const {
        auto it = stopIdToName.find(id);
        if (it != stopIdToName.end()) {
            return it->second;
        }
        return "";
    }

    // Для использования в Trip::deserialize
    friend class Trip;
};

// Реализация Trip::deserialize после определения TransportSystem
std::shared_ptr<Trip> Trip::deserialize(const std::string& data, TransportSystem* system) {
    std::istringstream ss(data);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(ss, token, '|')) {
        tokens.push_back(token);
    }

    if (tokens.size() < 6) {
        throw TransportException("Некорректные данные рейса");
    }

    int tripId = std::stoi(tokens[0]);
    auto route = Route::deserialize(tokens[1]);

    // Для транспортного средства ищем его в системе
    std::shared_ptr<Vehicle> vehicle = nullptr;
    if (system) {
        // Извлекаем номерной знак из сериализованных данных
        std::istringstream vehicleStream(tokens[2]);
        std::string type, model, licensePlate;
        std::getline(vehicleStream, type, '|');
        std::getline(vehicleStream, model, '|');
        std::getline(vehicleStream, licensePlate);

        // Ищем транспорт в системе
        vehicle = system->findVehicleByLicensePlate(licensePlate);
    }

    auto driver = Driver::deserialize(tokens[3]);
    Time startTime = Time::deserialize(tokens[4]);

    if (!vehicle) {
        throw TransportException("Транспортное средство не найдено в системе");
    }

    auto trip = std::make_shared<Trip>(tripId, route, vehicle, driver, startTime);

    // Десериализация расписания
    if (!tokens[5].empty()) {
        std::istringstream scheduleStream(tokens[5]);
        std::string scheduleItem;
        while (std::getline(scheduleStream, scheduleItem, ';')) {
            size_t eqPos = scheduleItem.find('=');
            if (eqPos != std::string::npos) {
                std::string stop = scheduleItem.substr(0, eqPos);
                Time time = Time::deserialize(scheduleItem.substr(eqPos + 1));
                trip->setArrivalTime(stop, time);
            }
        }
    }

    return trip;
}

// Реализация методов DataManager
void DataManager::saveAllData(TransportSystem& system) {
    try {
        std::cout << "Сохранение данных в текстовые файлы...\n";

        saveStops(system);
        saveVehicles(system);
        saveDrivers(system);
        saveRoutes(system);
        saveTrips(system);
        saveAdminCredentials(system);

        std::cout << "Данные успешно сохранены!\n";
    } catch (const std::exception& e) {
        std::cout << "Ошибка при сохранении данных: " << e.what() << "\n";
    }
}

void DataManager::loadAllData(TransportSystem& system) {
    try {
        std::cout << "Загрузка данных из текстовых файлов...\n";

        loadStops(system);
        loadVehicles(system);
        loadDrivers(system);
        loadRoutes(system);
        loadTrips(system);
        loadAdminCredentials(system);

        std::cout << "Данные успешно загружены!\n";
    } catch (const std::exception& e) {
        std::cout << "Ошибка при загрузке данных: " << e.what() << "\n";
        std::cout << "Используются тестовые данные.\n";
    }
}

void DataManager::saveStops(TransportSystem& system) {
    std::ofstream file(dataDirectory + "stops.txt");
    if (!file.is_open()) throw TransportException("Не удалось открыть файл stops.txt");

    const auto& stops = system.getStops();
    for (const auto& stop : stops) {
        file << stop.serialize() << "\n";
    }
    file.close();
}

void DataManager::saveVehicles(TransportSystem& system) {
    std::ofstream file(dataDirectory + "vehicles.txt");
    if (!file.is_open()) throw TransportException("Не удалось открыть файл vehicles.txt");

    const auto& vehicles = system.getVehicles();
    for (const auto& vehicle : vehicles) {
        file << vehicle->serialize() << "\n";
    }
    file.close();
}

void DataManager::saveDrivers(TransportSystem& system) {
    std::ofstream file(dataDirectory + "drivers.txt");
    if (!file.is_open()) throw TransportException("Не удалось открыть файл drivers.txt");

    const auto& drivers = system.getDrivers();
    for (const auto& driver : drivers) {
        file << driver->serialize() << "\n";
    }
    file.close();
}

void DataManager::saveRoutes(TransportSystem& system) {
    std::ofstream file(dataDirectory + "routes.txt");
    if (!file.is_open()) throw TransportException("Не удалось открыть файл routes.txt");

    const auto& routes = system.getRoutes();
    for (const auto& route : routes) {
        file << route->serialize() << "\n";
    }
    file.close();
}

void DataManager::saveTrips(TransportSystem& system) {
    std::ofstream file(dataDirectory + "trips.txt");
    if (!file.is_open()) throw TransportException("Не удалось открыть файл trips.txt");

    const auto& trips = system.getTrips();
    for (const auto& trip : trips) {
        file << trip->serialize() << "\n";
    }
    file.close();
}

void DataManager::saveAdminCredentials(TransportSystem& system) {
    std::ofstream file(dataDirectory + "admins.txt");
    if (!file.is_open()) throw TransportException("Не удалось открыть файл admins.txt");

    file << "admin|admin123\n";
    file << "manager|manager123\n";
    file.close();
}

void DataManager::loadStops(TransportSystem& system) {
    std::ifstream file(dataDirectory + "stops.txt");
    if (!file.is_open()) return; // Файл может отсутствовать

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            system.addStop(Stop::deserialize(line));
        }
    }
    file.close();
}

void DataManager::loadVehicles(TransportSystem& system) {
    std::ifstream file(dataDirectory + "vehicles.txt");
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            try {
                std::istringstream ss(line);
                std::string type, model, licensePlate;
                std::getline(ss, type, '|');
                std::getline(ss, model, '|');
                std::getline(ss, licensePlate);

                std::shared_ptr<Vehicle> vehicle;
                if (type == "Автобус") {
                    vehicle = std::make_shared<Bus>(model, licensePlate);
                } else if (type == "Трамвай") {
                    vehicle = std::make_shared<Tram>(model, licensePlate);
                } else if (type == "Троллейбус") {
                    vehicle = std::make_shared<Trolleybus>(model, licensePlate);
                } else {
                    throw TransportException("Неизвестный тип транспорта: " + type);
                }

                system.addVehicle(vehicle);
            } catch (const std::exception& e) {
                std::cout << "Ошибка загрузки транспортного средства: " << e.what() << "\n";
            }
        }
    }
    file.close();
}

void DataManager::loadDrivers(TransportSystem& system) {
    std::ifstream file(dataDirectory + "drivers.txt");
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            system.addDriver(Driver::deserialize(line));
        }
    }
    file.close();
}

void DataManager::loadRoutes(TransportSystem& system) {
    std::ifstream file(dataDirectory + "routes.txt");
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            try {
                system.addRoute(Route::deserialize(line));
            } catch (const std::exception& e) {
                std::cout << "Ошибка загрузки маршрута: " << e.what() << "\n";
            }
        }
    }
    file.close();
}

void DataManager::loadTrips(TransportSystem& system) {
    std::ifstream file(dataDirectory + "trips.txt");
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            try {
                system.addTrip(Trip::deserialize(line, &system));
            } catch (const std::exception& e) {
                std::cout << "Ошибка загрузки рейса: " << e.what() << "\n";
            }
        }
    }
    file.close();
}

void DataManager::loadAdminCredentials(TransportSystem& system) {
    std::ifstream file(dataDirectory + "admins.txt");
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            std::istringstream ss(line);
            std::string username, password;
            std::getline(ss, username, '|');
            std::getline(ss, password);
            system.addAdmin(username, password);
        }
    }
    file.close();
}

// Реализация методов JourneyPlanner
std::vector<Journey> JourneyPlanner::findJourneysWithTransfers(
    const std::string& startStop,
    const std::string& endStop,
    const Time& departureTime,
    int maxTransfers) const {

    std::vector<Journey> journeys;

    // Структура для BFS поиска
    struct SearchNode {
        std::string currentStop;
        Time currentTime;
        std::vector<std::shared_ptr<Trip>> pathTrips;
        std::vector<std::string> transferPoints;
        int transfers;
    };

    std::queue<SearchNode> q;
    q.push({startStop, departureTime, {}, {}, 0});

    while (!q.empty()) {
        auto node = q.front();
        q.pop();

        // Если достигли конечной остановки
        if (node.currentStop == endStop) {
            journeys.emplace_back(node.pathTrips, node.transferPoints,
                                 departureTime, node.currentTime);
            continue;
        }

        // Если превысили лимит пересадок
        if (node.transfers >= maxTransfers) {
            continue;
        }

        // Ищем все рейсы, проходящие через текущую остановку
        auto trips = system->getTripsThroughStop(node.currentStop);

        for (const auto& trip : trips) {
            Time arrivalAtStop = trip->getArrivalTime(node.currentStop);

            // Пропускаем, если рейс уже ушел
            if (arrivalAtStop < node.currentTime) {
                continue;
            }

            // Пропускаем, если этот рейс уже в пути
            if (!node.pathTrips.empty() && node.pathTrips.back() == trip) {
                continue;
            }

            // Получаем все остановки после текущей
            const auto& routeStops = trip->getRoute()->getAllStops();
            int currentPos = trip->getRoute()->getStopPosition(node.currentStop);

            if (currentPos == -1) continue;

            // Проверяем все возможные точки выхода
            for (int i = currentPos + 1; i < routeStops.size(); ++i) {
                std::string nextStop = routeStops[i];
                Time arrivalAtNext = trip->getArrivalTime(nextStop);

                // Создаем новый узел
                SearchNode nextNode = node;
                nextNode.currentStop = nextStop;
                nextNode.currentTime = arrivalAtNext;
                nextNode.pathTrips.push_back(trip);

                // Если это пересадка (не первый рейс в пути)
                if (!node.pathTrips.empty()) {
                    nextNode.transferPoints.push_back(node.currentStop);
                    nextNode.transfers++;
                }

                q.push(nextNode);
            }
        }
    }

    // Сортируем по времени в пути
    std::sort(journeys.begin(), journeys.end(),
              [](const Journey& a, const Journey& b) {
                  return a.getTotalDuration() < b.getTotalDuration();
              });

    return journeys;
}

Journey JourneyPlanner::findFastestJourney(const std::string& startStop,
                                          const std::string& endStop,
                                          const Time& departureTime) {
    auto journeys = findJourneysWithTransfers(startStop, endStop, departureTime, 2);

    if (journeys.empty()) {
        throw TransportException("Маршрут не найден");
    }

    // Первый в списке самый быстрый (после сортировки)
    return journeys[0];
}

Journey JourneyPlanner::findJourneyWithLeastTransfers(const std::string& startStop,
                                                     const std::string& endStop,
                                                     const Time& departureTime) {
    auto journeys = findJourneysWithTransfers(startStop, endStop, departureTime, 2);

    if (journeys.empty()) {
        throw TransportException("Маршрут не найден");
    }

    // Ищем с минимальным количеством пересадок
    auto it = std::min_element(journeys.begin(), journeys.end(),
                               [](const Journey& a, const Journey& b) {
                                   return a.getTransferCount() < b.getTransferCount();
                               });

    return *it;
}

// Функции для пользовательского интерфейса
void displayGuestMenu() {
    std::cout << "\n=== ГОСТЕВОЙ РЕЖИМ ===\n";
    std::cout << "1. Поиск маршрутов между остановками\n";
    std::cout << "2. Просмотр расписания остановки\n";
    std::cout << "3. Расчет времени прибытия для рейса\n";
    std::cout << "4. Поиск маршрута с пересадками\n";
    std::cout << "5. Показать все рейсы\n";
    std::cout << "6. Сохранить данные\n";
    std::cout << "7. Выход\n";
    std::cout << "Выберите опцию: ";
}

void displayAdminMenu() {
    std::cout << "\n=== АДМИНИСТРАТИВНЫЙ РЕЖИМ ===\n";
    std::cout << "1. Поиск маршрутов между остановками\n";
    std::cout << "2. Просмотр расписания остановки\n";
    std::cout << "3. Расчет времени прибытия для рейса\n";
    std::cout << "4. Поиск маршрута с пересадками\n";
    std::cout << "5. Показать все рейсы\n";
    std::cout << "--- АДМИНИСТРАТИВНЫЕ ФУНКЦИИ ---\n";
    std::cout << "6. Добавить маршрут\n";
    std::cout << "7. Добавить рейс\n";
    std::cout << "8. Добавить транспортное средство\n";
    std::cout << "9. Добавить остановку\n";
    std::cout << "10. Добавить водителя\n";
    std::cout << "11. Удалить маршрут\n";
    std::cout << "12. Удалить рейс\n";
    std::cout << "13. Просмотр всех данных\n";
    std::cout << "14. Сохранить данные\n";
    std::cout << "15. Выход\n";
    std::cout << "Выберите опцию: ";
}

void displayLoginMenu() {
    std::cout << "\n=== СИСТЕМА РАСПИСАНИЯ ГОРОДСКОГО ТРАНСПОРТА ===\n";
    std::cout << "1. Войти как администратор\n";
    std::cout << "2. Войти как гость\n";
    std::cout << "3. Сохранить и выйти\n";
    std::cout << "Выберите опцию: ";
}

void displayAllStopsForSelection(const TransportSystem& system) {
    const auto& stops = system.getStops();
    std::cout << "\n=== СПИСОК ДОСТУПНЫХ ОСТАНОВКИ ===\n";
    for (const auto& stop : stops) {
        std::cout << "• " << stop.getName() << " (ID: " << stop.getId() << ")\n";
    }
    std::cout << "=================================\n";
}

// Функции для административных операций
void adminAddRoute(TransportSystem& system) {
    try {
        int number;
        std::string vehicleType;
        std::vector<std::string> stops;
        std::string stop;

        std::cout << "Введите номер маршрута: ";
        std::cin >> number;
        std::cin.ignore();

        std::cout << "Введите тип транспорта (Автобус/Трамвай/Троллейбус): ";
        std::getline(std::cin, vehicleType);

        std::cout << "Введите остановки маршрута (введите 'конец' для завершения):\n";
        while (true) {
            std::cout << "Остановка: ";
            std::getline(std::cin, stop);
            if (stop == "конец") break;
            stops.push_back(stop);
        }

        auto route = std::make_shared<Route>(number, vehicleType, stops);
        system.addRoute(route);
        std::cout << "Маршрут успешно добавлен!\n";

    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << '\n';
    }
}

void adminAddTrip(TransportSystem& system) {
    try {
        // Покажем доступные маршруты
        std::cout << "\n=== ДОСТУПНЫЕ МАРШРУТЫ ===\n";
        system.displayAllRoutes();
        std::cout << "==========================\n\n";

        // Покажем доступный транспорт
        std::cout << "=== ДОСТУПНЫЙ ТРАНСПОРТ ===\n";
        system.displayAllVehicles();
        std::cout << "===========================\n\n";

        // Покажем доступных водителей
        std::cout << "=== ДОСТУПНЫЕ ВОДИТЕЛИ ===\n";
        const auto& drivers = system.getDrivers();
        for (size_t i = 0; i < drivers.size(); ++i) {
            std::cout << (i+1) << ". " << drivers[i]->getFullName() << '\n';
        }
        std::cout << "==========================\n\n";

        int tripId, routeNumber;
        std::string licensePlate, driverFirstName, driverLastName, driverMiddleName, startTimeStr;

        std::cout << "Введите ID рейса: ";
        std::cin >> tripId;
        std::cin.ignore();

        std::cout << "Введите номер маршрута: ";
        std::cin >> routeNumber;
        std::cin.ignore();

        auto route = system.findRouteByNumber(routeNumber);
        if (!route) {
            throw TransportException("Маршрут не найден!");
        }

        std::cout << "Введите номерной знак транспортного средства: ";
        std::getline(std::cin, licensePlate);

        auto vehicle = system.findVehicleByLicensePlate(licensePlate);
        if (!vehicle) {
            throw TransportException("Транспортное средство не найдено!");
        }

        std::cout << "Введите имя водителя: ";
        std::getline(std::cin, driverFirstName);
        std::cout << "Введите фамилию водителя: ";
        std::getline(std::cin, driverLastName);
        std::cout << "Введите отчество водителя (если есть, иначе Enter): ";
        std::getline(std::cin, driverMiddleName);

        auto driver = system.findDriverByName(driverFirstName, driverLastName, driverMiddleName);
        if (!driver) {
            throw TransportException("Водитель не найден!");
        }

        std::cout << "Введите время отправления (HH:MM): ";
        std::getline(std::cin, startTimeStr);

        Time startTime(startTimeStr);
        auto trip = std::make_shared<Trip>(tripId, route, vehicle, driver, startTime);
        system.addTrip(trip);
        std::cout << "Рейс успешно добавлен!\n";

    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << '\n';
    }
}

void adminAddVehicle(TransportSystem& system) {
    try {
        std::string type, model, licensePlate;

        std::cout << "Введите тип транспорта (Автобус/Трамвай/Троллейбус): ";
        std::getline(std::cin, type);

        std::cout << "Введите модель: ";
        std::getline(std::cin, model);

        std::cout << "Введите номерной знак: ";
        std::getline(std::cin, licensePlate);

        std::shared_ptr<Vehicle> vehicle;
        if (type == "Автобус") {
            vehicle = std::make_shared<Bus>(model, licensePlate);
        } else if (type == "Трамвай") {
            vehicle = std::make_shared<Tram>(model, licensePlate);
        } else if (type == "Троллейбус") {
            vehicle = std::make_shared<Trolleybus>(model, licensePlate);
        }

        system.addVehicle(vehicle);
        std::cout << "Транспортное средство успешно добавлено!\n";

    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << '\n';
    }
}

void adminAddStop(TransportSystem& system) {
    try {
        int id;
        std::string name;

        std::cout << "Введите ID остановки: ";
        std::cin >> id;
        std::cin.ignore();

        std::cout << "Введите название остановки: ";
        std::getline(std::cin, name);

        system.addStop(Stop(id, name));
        std::cout << "Остановка успешно добавлена!\n";

    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << '\n';
    }
}

void adminAddDriver(TransportSystem& system) {
    try {
        std::string firstName, lastName, middleName;

        std::cout << "Введите имя водителя: ";
        std::getline(std::cin, firstName);
        std::cout << "Введите фамилию водителя: ";
        std::getline(std::cin, lastName);
        std::cout << "Введите отчество водителя (если есть, иначе Enter): ";
        std::getline(std::cin, middleName);

        auto driver = std::make_shared<Driver>(firstName, lastName, middleName);
        system.addDriver(driver);
        std::cout << "Водитель успешно добавлен!\n";

    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << '\n';
    }
}

// Инициализация тестовых данных
void initializeTestData(TransportSystem& system) {
    system.addStop(Stop(1, "Центральный вокзал"));
    system.addStop(Stop(2, "Площадь Ленина"));
    system.addStop(Stop(3, "Улица Гагарина"));
    system.addStop(Stop(4, "Парк Победы"));
    system.addStop(Stop(5, "Стадион"));
    system.addStop(Stop(6, "Больница"));
    system.addStop(Stop(7, "Университет"));

    auto bus1 = std::make_shared<Bus>("МАЗ-203", "АН 8669-7");
    auto bus2 = std::make_shared<Bus>("ПАЗ-3205", "ВС 1234-5");
    auto tram1 = std::make_shared<Tram>("71-931", "ТР 5678-9");

    system.addVehicle(bus1);
    system.addVehicle(bus2);
    system.addVehicle(tram1);

    auto driver1 = std::make_shared<Driver>("Иван", "Петров", "Сергеевич");
    auto driver2 = std::make_shared<Driver>("Мария", "Сидорова", "Ивановна");
    auto driver3 = std::make_shared<Driver>("Алексей", "Козлов");

    system.addDriver(driver1);
    system.addDriver(driver2);
    system.addDriver(driver3);

    std::vector<std::string> route1Stops = {"Центральный вокзал", "Площадь Ленина", "Улица Гагарина", "Стадион"};
    auto route1 = std::make_shared<Route>(101, "Автобус", route1Stops);

    std::vector<std::string> route2Stops = {"Центральный вокзал", "Площадь Ленина", "Больница", "Университет"};
    auto route2 = std::make_shared<Route>(202, "Автобус", route2Stops);

    std::vector<std::string> route3Stops = {"Парк Победы", "Улица Гагарина", "Больница", "Университет"};
    auto route3 = std::make_shared<Route>(5, "Трамвай", route3Stops);

    system.addRoute(route1);
    system.addRoute(route2);
    system.addRoute(route3);

    // Создаем тестовые рейсы
    try {
        auto trip1 = std::make_shared<Trip>(1, route1, bus1, driver1, Time("08:00"));
        auto trip2 = std::make_shared<Trip>(2, route2, bus2, driver2, Time("09:00"));
        auto trip3 = std::make_shared<Trip>(3, route3, tram1, driver3, Time("10:00"));

        system.addTrip(trip1);
        system.addTrip(trip2);
        system.addTrip(trip3);

        // Рассчитываем время прибытия - ИСПРАВЛЕННЫЙ ВЫЗОВ
        system.calculateArrivalTimes(1, 30.0);
        system.calculateArrivalTimes(2, 30.0);
        system.calculateArrivalTimes(3, 25.0);

    } catch (const std::exception& e) {
        std::cout << "Ошибка при создании тестовых рейсов: " << e.what() << "\n";
    }
}

// Функции для выполнения общих операций
void searchRoutes(TransportSystem& system) {
    try {
        std::string stopA, stopB;

        // Показываем список остановок
        displayAllStopsForSelection(system);

        std::cout << "\nВведите начальную остановку: ";
        std::getline(std::cin, stopA);
        std::cout << "Введите конечную остановку: ";
        std::getline(std::cin, stopB);

        auto routes = system.findRoutes(stopA, stopB);
        std::cout << "\nНайдено маршрутов: " << routes.size() << '\n';

        if (routes.empty()) {
            std::cout << "Прямых маршрутов не найдено. Попробуйте поиск с пересадками.\n";
        } else {
            for (const auto& route : routes) {
                std::cout << "\nМаршрут " << route->getNumber() << " ("
                          << route->getVehicleType() << ")\n";
                std::cout << "Весь путь: " << route->getStartStop() << " → ";
                for (size_t i = 1; i < route->getAllStops().size() - 1; i++) {
                    std::cout << route->getAllStops()[i] << " → ";
                }
                std::cout << route->getEndStop() << '\n';
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << "\n";
    }
}

void viewStopTimetable(TransportSystem& system) {
    try {
        // Показываем список остановок
        displayAllStopsForSelection(system);

        int stopId;
        Time startTime, endTime;

        std::cout << "\nВведите ID остановки: ";
        std::cin >> stopId;
        std::cout << "Введите начальное время (HH:MM): ";
        std::cin >> startTime;
        std::cout << "Введите конечное время (HH:MM): ";
        std::cin >> endTime;
        std::cin.ignore();

        system.getStopTimetable(stopId, startTime, endTime);
    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << '\n';
    }
}

// Исправленная функция расчета времени прибытия
void calculateArrivalTime(TransportSystem& system) {
    try {
        // Покажем доступные рейсы
        const auto& trips = system.getTrips();
        if (trips.empty()) {
            std::cout << "В системе нет рейсов.\n";
            return;
        }

        std::cout << "\n=== ДОСТУПНЫЕ РЕЙСЫ ===\n";
        for (const auto& trip : trips) {
            std::cout << "ID: " << trip->getTripId()
                      << " | Маршрут: " << trip->getRoute()->getNumber()
                      << " | Отправление: " << trip->getStartTime()
                      << " | Направление: " << trip->getRoute()->getStartStop()
                      << " → " << trip->getRoute()->getEndStop() << '\n';
        }
        std::cout << "========================\n\n";

        int tripId;
        double speed;

        std::cout << "Введите ID рейса: ";
        std::cin >> tripId;
        std::cout << "Введите среднюю скорость (км/ч): ";
        std::cin >> speed;
        std::cin.ignore();

        system.calculateArrivalTimes(tripId, speed);

        // Покажем обновленное расписание
        auto tripIt = std::find_if(trips.begin(), trips.end(),
                                  [tripId](const auto& t) { return t->getTripId() == tripId; });
        if (tripIt != trips.end()) {
            auto trip = *tripIt;
            std::cout << "\nОбновленное расписание для рейса " << tripId << ":\n";
            const auto& schedule = trip->getSchedule();
            for (const auto& [stop, time] : schedule) {
                std::cout << "  " << stop << " - " << time << '\n';
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << "\n";
    }
}

void searchRoutesWithTransfers(TransportSystem& system) {
    try {
        std::string start, end;
        Time departure;

        // Показываем список остановок
        displayAllStopsForSelection(system);

        std::cout << "\nОткуда: ";
        std::getline(std::cin, start);
        std::cout << "Куда: ";
        std::getline(std::cin, end);
        std::cout << "Во сколько выезжаете (HH:MM): ";
        std::cin >> departure;
        std::cin.ignore();

        auto journeys = system.getJourneyPlanner().findJourneysWithTransfers(start, end, departure, 2);
        if (journeys.empty()) {
            std::cout << "Маршрутов не найдено!\n";
        } else {
            std::cout << "\nНайдено " << journeys.size() << " вариантов:\n";
            for (size_t i = 0; i < std::min(journeys.size(), size_t(3)); i++) {
                std::cout << "\n════════════════════════════════════════\n";
                std::cout << "Вариант " << (i+1) << ":\n";
                system.getJourneyPlanner().displayJourney(journeys[i]);
            }
            std::cout << "════════════════════════════════════════\n";
        }
    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << '\n';
    }
}

void showAllTrips(const TransportSystem& system) {
    const auto& trips = system.getTrips();
    std::cout << "\nВсе рейсы в системе:\n";
    for (const auto& trip : trips) {
        std::cout << "Рейс " << trip->getTripId()
                  << ": Маршрут " << trip->getRoute()->getNumber()
                  << ", отправление: " << trip->getStartTime()
                  << ", водитель: " << trip->getDriver()->getFullName()
                  << ", транспорт: " << trip->getVehicle()->getLicensePlate() << '\n';
    }
}

// Основные функции режимов работы
void runGuestMode(TransportSystem& system) {
    int choice;
    bool running = true;

    while (running) {
        displayGuestMenu();
        std::cin >> choice;
        std::cin.ignore();

        try {
            switch (choice) {
                case 1: searchRoutes(system); break;
                case 2: viewStopTimetable(system); break;
                case 3: calculateArrivalTime(system); break;
                case 4: searchRoutesWithTransfers(system); break;
                case 5: showAllTrips(system); break;
                case 6: system.saveData(); break;
                case 7: running = false; break;
                default: std::cout << "Неверный выбор.\n";
            }
        } catch (const std::exception& e) {
            std::cout << "Ошибка: " << e.what() << '\n';
        }
    }
}

void runAdminMode(TransportSystem& system) {
    std::string username, password;

    std::cout << "Введите логин: ";
    std::getline(std::cin, username);
    std::cout << "Введите пароль: ";
    std::getline(std::cin, password);

    if (!system.authenticateAdmin(username, password)) {
        std::cout << "Неверный логин или пароль!\n";
        return;
    }

    std::cout << "Успешный вход!\n";

    int choice;
    bool running = true;

    while (running) {
        displayAdminMenu();
        std::cin >> choice;
        std::cin.ignore();

        try {
            switch (choice) {
                case 1: searchRoutes(system); break;
                case 2: viewStopTimetable(system); break;
                case 3: calculateArrivalTime(system); break;
                case 4: searchRoutesWithTransfers(system); break;
                case 5: showAllTrips(system); break;
                case 6: adminAddRoute(system); break;
                case 7: adminAddTrip(system); break;
                case 8: adminAddVehicle(system); break;
                case 9: adminAddStop(system); break;
                case 10: adminAddDriver(system); break;
                case 11: {
                    int routeNumber;
                    std::cout << "Введите номер маршрута для удаления: ";
                    std::cin >> routeNumber;
                    std::cin.ignore();
                    system.removeRoute(routeNumber);
                    break;
                }
                case 12: {
                    int tripId;
                    std::cout << "Введите ID рейса для удаления: ";
                    std::cin >> tripId;
                    std::cin.ignore();
                    system.removeTrip(tripId);
                    break;
                }
                case 13: {
                    system.displayAllRoutes();
                    system.displayAllTrips();
                    system.displayAllVehicles();
                    system.displayAllStops();
                    break;
                }
                case 14: system.saveData(); break;
                case 15: running = false; break;
                default: std::cout << "Неверный выбор.\n";
            }
        } catch (const std::exception& e) {
            std::cout << "Ошибка: " << e.what() << '\n';
        }
    }
}

int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    try {
        TransportSystem system;

        // Пытаемся загрузить данные из файлов
        system.loadData();

        // Если данные не загрузились, создаем тестовые данные
        if (system.getStops().empty()) {
            std::cout << "Создание тестовых данных...\n";
            initializeTestData(system);
        }

        int choice;
        bool running = true;

        while (running) {
            displayLoginMenu();
            std::cin >> choice;
            std::cin.ignore();

            switch (choice) {
                case 1: runAdminMode(system); break;
                case 2: runGuestMode(system); break;
                case 3:
                    system.saveData();
                    running = false;
                    std::cout << "Данные сохранены. Выход из программы.\n";
                    break;
                default: std::cout << "Неверный выбор.\n";
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Критическая ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
#include "Menu.h"
#include <iostream>
#include <limits>

void displayMainMenu() {
    std::cout << "\n=== СИСТЕМА ОБЩЕСТВЕННОГО ТРАНСПОРТА ===\n";
    std::cout << "1. Поиск маршрутов между остановками\n";
    std::cout << "2. Просмотр расписания остановки\n";
    std::cout << "3. Показать все маршруты\n";
    std::cout << "4. Управление данными\n";
    std::cout << "5. Выход\n";
    std::cout << "Выберите опцию: ";
}

void displayDataMenu() {
    std::cout << "\n=== УПРАВЛЕНИЕ ДАННЫМИ ===\n";
    std::cout << "1. Добавить маршрут\n";
    std::cout << "2. Добавить рейс\n";
    std::cout << "3. Удалить маршрут\n";
    std::cout << "4. Найти маршруты через остановку\n";
    std::cout << "5. Назад\n";
    std::cout << "Выберите опцию: ";
}

void searchRoutes(TransportSystem& system) {
    std::string from, to;
    std::cout << "Введите начальную остановку: ";
    std::getline(std::cin, from);
    std::cout << "Введите конечную остановку: ";
    std::getline(std::cin, to);

    auto routes = system.findRoutes(from, to);
    std::cout << "\nНайдено маршрутов: " << routes.size() << '\n';
    for (const auto& route : routes) {
        std::cout << "Маршрут " << route->getNumber() << " ("
                  << route->getVehicleType() << ")\n";
        std::cout << "  " << route->getStartStop() << " -> " << route->getEndStop() << "\n";
    }
}

void viewSchedule(TransportSystem& system) {
    std::string stopName;
    std::string fromStr, toStr;

    std::cout << "Введите название остановки: ";
    std::getline(std::cin, stopName);
    std::cout << "Введите начало периода (HH:MM): ";
    std::getline(std::cin, fromStr);
    std::cout << "Введите конец периода (HH:MM): ";
    std::getline(std::cin, toStr);

    Time fromTime, toTime;
    if (!fromTime.parseFromString(fromStr) || !toTime.parseFromString(toStr)) {
        std::cout << "Ошибка ввода времени" << std::endl;
        return;
    }

    system.showStopSchedule(stopName, fromTime, toTime);
}

void showAllRoutes(TransportSystem& system) {
    auto routes = system.getAllRoutesList();
    std::cout << "\n=== ВСЕ МАРШРУТЫ ===\n";
    for (const auto& route : routes) {
        std::cout << "Маршрут " << route->getNumber() << " (" << route->getVehicleType() << ")\n";
        std::cout << "  Остановки: ";
        const auto& stops = route->getStops();
        bool first = true;
        for (const auto& stop : stops) {
            if (!first) std::cout << " → ";
            std::cout << stop;
            first = false;
        }
        std::cout << "\n";
    }
}

void addRouteInteractive(TransportSystem& system) {
    int number;
    std::string vehicleType;
    std::list<std::string> stops;
    std::string stop;

    std::cout << "Введите номер маршрута: ";
    std::cin >> number;
    std::cin.ignore();

    std::cout << "Введите тип транспорта: ";
    std::getline(std::cin, vehicleType);

    std::cout << "Введите остановки (введите 'конец' для завершения):\n";
    while (true) {
        std::cout << "Остановка: ";
        std::getline(std::cin, stop);
        if (stop == "конец") break;
        if (!stop.empty()) {
            stops.push_back(stop);
        }
    }

    auto route = std::make_shared<Route>(number, vehicleType, stops);
    if (system.addRoute(route)) {
        std::cout << "Маршрут успешно добавлен!\n";
    }
}

void addTripInteractive(TransportSystem& system) {
    int tripId, routeNumber;
    std::string startTimeStr;

    std::cout << "Введите ID рейса: ";
    std::cin >> tripId;
    std::cout << "Введите номер маршрута: ";
    std::cin >> routeNumber;
    std::cin.ignore();

    std::cout << "Введите время отправления (HH:MM): ";
    std::getline(std::cin, startTimeStr);

    // Поиск маршрута
    auto route = system.findRouteByNumber(routeNumber);
    if (!route) {
        std::cout << "Ошибка: маршрут не найден!\n";
        return;
    }

    Time startTime;
    if (!startTime.parseFromString(startTimeStr)) {
        std::cout << "Ошибка: некорректное время отправления!\n";
        return;
    }

    auto trip = std::make_shared<Trip>(tripId, route, startTime);
    if (system.addTrip(trip)) {
        std::cout << "Рейс успешно добавлен!\n";
    }
}

void removeRouteInteractive(TransportSystem& system) {
    int number;
    std::cout << "Введите номер маршрута для удаления: ";
    std::cin >> number;
    std::cin.ignore();

    system.removeRoute(number);
}

void findRoutesThroughStop(TransportSystem& system) {
    std::string stop;
    std::cout << "Введите название остановки: ";
    std::getline(std::cin, stop);

    auto routes = system.findRoutesThroughStop(stop);
    std::cout << "\nМаршруты через остановку '" << stop << "': " << routes.size() << '\n';
    for (const auto& route : routes) {
        std::cout << "Маршрут " << route->getNumber() << " (" << route->getVehicleType() << ")\n";
    }
}

void runDataManagement(TransportSystem& system) {
    int choice;
    bool running = true;

    while (running) {
        displayDataMenu();
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
            case 1: addRouteInteractive(system); break;
            case 2: addTripInteractive(system); break;
            case 3: removeRouteInteractive(system); break;
            case 4: findRoutesThroughStop(system); break;
            case 5: running = false; break;
            default: std::cout << "Неверный выбор!\n";
        }
    }
}

void initializeTestData(TransportSystem& system) {
    // Маршруты
    auto route1 = std::make_shared<Route>(101, "Автобус",
        std::list<std::string>{"Вокзал", "Центр", "Университет", "Стадион"});
    auto route2 = std::make_shared<Route>(202, "Трамвай",
        std::list<std::string>{"Вокзал", "Центр", "Больница", "Парк"});

    system.addRoute(route1);
    system.addRoute(route2);

    // Рейсы
    Time startTime1, startTime2, startTime3;
    startTime1.parseFromString("08:00");
    startTime2.parseFromString("09:00");
    startTime3.parseFromString("08:30");

    auto trip1 = std::make_shared<Trip>(1, route1, startTime1);
    auto trip2 = std::make_shared<Trip>(2, route1, startTime2);
    auto trip3 = std::make_shared<Trip>(3, route2, startTime3);

    system.addTrip(trip1);
    system.addTrip(trip2);
    system.addTrip(trip3);
}
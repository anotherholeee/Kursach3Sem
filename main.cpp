#include <iostream>
#include <windows.h>
#include <limits>
#include "TransportSystem.h"
#include "Menu.h"

int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    TransportSystem system;

    initializeTestData(system);

    int choice;
    bool running = true;

    while (running) {
        displayMainMenu();
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: searchRoutes(system); break;
            case 2: viewSchedule(system); break;
            case 3: showAllRoutes(system); break;
            case 4: runDataManagement(system); break;
            case 5: running = false; std::cout << "Выход...\n"; break;
            default: std::cout << "Неверный выбор!\n";
        }
    }

    return 0;
}
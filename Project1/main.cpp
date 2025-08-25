#include "EventManager.h"
#include <iostream>
#include <chrono>

int main() {
    try {
        EventManager manager("C:\\TOP\\EventManager\\events.txt");


        std::chrono::sys_days newEventDate = std::chrono::sys_days{ std::chrono::year{2023} / std::chrono::month{12} / std::chrono::day{25} };
        manager.addEvent(Event(newEventDate, "Sport"));

        // Сохранение изменений в файл
        manager.saveToFile("C:\TOP\EventManager\events.txt");

        // Получение ближайших событий
        auto upcomingEvents = manager.getUpcomingEvents(std::chrono::sys_days{ std::chrono::year{2023} / std::chrono::month{10} / std::chrono::day{1} });
        for (const auto& event : upcomingEvents) {
            std::cout << event.getDescription() << " on " << event.getDate() << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
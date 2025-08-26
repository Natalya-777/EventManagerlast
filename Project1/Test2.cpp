#include "pch.h"
#include "EventManager.h"
#include "Event.h"
#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <chrono>

using namespace std::chrono;

class EventManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Создаем временный файл для тестов
        testFilename = "test_events.txt";

        // Очищаем файл перед каждым тестом
        std::ofstream file(testFilename, std::ofstream::trunc);
    }

    void TearDown() override {
        // Удаляем временный файл после тестов
        if (std::filesystem::exists(testFilename)) {
            std::filesystem::remove(testFilename);
        }
    }

    std::string testFilename;
};

TEST_F(EventManagerTest, AddEvent_AddsEventToVector) {
    EventManager manager(testFilename);

    // Создаем событие
    sys_days date = sys_days{ year{2023} / month{12} / day{25} };
    Event event(date, "Christmas Party");

    // Добавляем событие
    manager.addEvent(event);

    // Проверяем, что событие добавлено
    auto upcoming = manager.getUpcomingEvents(sys_days{ year{2023} / month{1} / day{1} }, 10);
    ASSERT_EQ(upcoming.size(), 1);
    EXPECT_EQ(upcoming[0].getDescription(), "Christmas Party");
}

TEST_F(EventManagerTest, AddEvent_SortsEventsByDate) {
    EventManager manager(testFilename);

    // Создаем события в разном порядке
    Event event1(sys_days{ year{2023} / month{12} / day{25} }, "Christmas");
    Event event2(sys_days{ year{2023} / month{10} / day{31} }, "Halloween");
    Event event3(sys_days{ 2023y / November / 11d }, "Veterans Day");

    // Добавляем в неправильном порядке
    manager.addEvent(event1); // 25 декабря
    manager.addEvent(event2); // 31 октября
    manager.addEvent(event3); // 11 ноября

    // Проверяем, что события отсортированы по дате
    auto upcoming = manager.getUpcomingEvents(sys_days{ year{2023} / month{1} / day{1} }, 10);

    ASSERT_EQ(upcoming.size(), 3);
    EXPECT_EQ(upcoming[0].getDescription(), "Halloween");    // 31 октября
    EXPECT_EQ(upcoming[1].getDescription(), "Veterans Day"); // 11 ноября
    EXPECT_EQ(upcoming[2].getDescription(), "Christmas");    // 25 декабря
}

TEST_F(EventManagerTest, SaveToFile_SavesEventsCorrectly) {
    EventManager manager(testFilename);

    // Добавляем тестовые события
    manager.addEvent(Event(sys_days{ 2023y / December / 25d }, "Christmas"));
    manager.addEvent(Event(sys_days{ 2023y / October / 31d }, "Halloween"));

    // Сохраняем в файл
    manager.saveToFile(testFilename);

    // Читаем файл и проверяем содержимое
    std::ifstream file(testFilename);
    ASSERT_TRUE(file.is_open());

    std::string line;
    std::vector<std::string> lines;

    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    // Проверяем, что файл содержит 2 строки
    ASSERT_EQ(lines.size(), 2);

    // Проверяем формат дат (сортировка должна быть по дате)
    EXPECT_TRUE(lines[0].find("31.10.2023 Halloween") != std::string::npos);
    EXPECT_TRUE(lines[1].find("25.12.2023 Christmas") != std::string::npos);
}

TEST_F(EventManagerTest, SaveToFile_EmptyVector_SavesEmptyFile) {
    EventManager manager(testFilename);

    // Сохраняем пустой вектор
    manager.saveToFile(testFilename);

    // Проверяем, что файл пустой
    std::ifstream file(testFilename);
    ASSERT_TRUE(file.is_open());

    std::string line;
    std::vector<std::string> lines;

    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    EXPECT_EQ(lines.size(), 0);
}

TEST_F(EventManagerTest, AddAndSaveIntegration) {
    EventManager manager(testFilename);

    // Добавляем несколько событий
    manager.addEvent(Event(sys_days{ 2024y / January / 1d }, "New Year"));
    manager.addEvent(Event(sys_days{ 2023y / December / 31d }, "New Year's Eve"));

    // Сохраняем
    manager.saveToFile(testFilename);

    // Создаем новый менеджер и загружаем из файла
    EventManager manager2(testFilename);

    // Проверяем, что события загрузились правильно и отсортированы
    auto events = manager2.getUpcomingEvents(sys_days{ 2023y / January / 1d }, 10);

    ASSERT_EQ(events.size(), 2);
    EXPECT_EQ(events[0].getDescription(), "New Year's Eve"); // 31 декабря
    EXPECT_EQ(events[1].getDescription(), "New Year");       // 1 января
}
Настр
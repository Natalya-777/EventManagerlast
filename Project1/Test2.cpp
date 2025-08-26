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
        // ������� ��������� ���� ��� ������
        testFilename = "test_events.txt";

        // ������� ���� ����� ������ ������
        std::ofstream file(testFilename, std::ofstream::trunc);
    }

    void TearDown() override {
        // ������� ��������� ���� ����� ������
        if (std::filesystem::exists(testFilename)) {
            std::filesystem::remove(testFilename);
        }
    }

    std::string testFilename;
};

TEST_F(EventManagerTest, AddEvent_AddsEventToVector) {
    EventManager manager(testFilename);

    // ������� �������
    sys_days date = sys_days{ year{2023} / month{12} / day{25} };
    Event event(date, "Christmas Party");

    // ��������� �������
    manager.addEvent(event);

    // ���������, ��� ������� ���������
    auto upcoming = manager.getUpcomingEvents(sys_days{ year{2023} / month{1} / day{1} }, 10);
    ASSERT_EQ(upcoming.size(), 1);
    EXPECT_EQ(upcoming[0].getDescription(), "Christmas Party");
}

TEST_F(EventManagerTest, AddEvent_SortsEventsByDate) {
    EventManager manager(testFilename);

    // ������� ������� � ������ �������
    Event event1(sys_days{ year{2023} / month{12} / day{25} }, "Christmas");
    Event event2(sys_days{ year{2023} / month{10} / day{31} }, "Halloween");
    Event event3(sys_days{ 2023y / November / 11d }, "Veterans Day");

    // ��������� � ������������ �������
    manager.addEvent(event1); // 25 �������
    manager.addEvent(event2); // 31 �������
    manager.addEvent(event3); // 11 ������

    // ���������, ��� ������� ������������� �� ����
    auto upcoming = manager.getUpcomingEvents(sys_days{ year{2023} / month{1} / day{1} }, 10);

    ASSERT_EQ(upcoming.size(), 3);
    EXPECT_EQ(upcoming[0].getDescription(), "Halloween");    // 31 �������
    EXPECT_EQ(upcoming[1].getDescription(), "Veterans Day"); // 11 ������
    EXPECT_EQ(upcoming[2].getDescription(), "Christmas");    // 25 �������
}

TEST_F(EventManagerTest, SaveToFile_SavesEventsCorrectly) {
    EventManager manager(testFilename);

    // ��������� �������� �������
    manager.addEvent(Event(sys_days{ 2023y / December / 25d }, "Christmas"));
    manager.addEvent(Event(sys_days{ 2023y / October / 31d }, "Halloween"));

    // ��������� � ����
    manager.saveToFile(testFilename);

    // ������ ���� � ��������� ����������
    std::ifstream file(testFilename);
    ASSERT_TRUE(file.is_open());

    std::string line;
    std::vector<std::string> lines;

    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    // ���������, ��� ���� �������� 2 ������
    ASSERT_EQ(lines.size(), 2);

    // ��������� ������ ��� (���������� ������ ���� �� ����)
    EXPECT_TRUE(lines[0].find("31.10.2023 Halloween") != std::string::npos);
    EXPECT_TRUE(lines[1].find("25.12.2023 Christmas") != std::string::npos);
}

TEST_F(EventManagerTest, SaveToFile_EmptyVector_SavesEmptyFile) {
    EventManager manager(testFilename);

    // ��������� ������ ������
    manager.saveToFile(testFilename);

    // ���������, ��� ���� ������
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

    // ��������� ��������� �������
    manager.addEvent(Event(sys_days{ 2024y / January / 1d }, "New Year"));
    manager.addEvent(Event(sys_days{ 2023y / December / 31d }, "New Year's Eve"));

    // ���������
    manager.saveToFile(testFilename);

    // ������� ����� �������� � ��������� �� �����
    EventManager manager2(testFilename);

    // ���������, ��� ������� ����������� ��������� � �������������
    auto events = manager2.getUpcomingEvents(sys_days{ 2023y / January / 1d }, 10);

    ASSERT_EQ(events.size(), 2);
    EXPECT_EQ(events[0].getDescription(), "New Year's Eve"); // 31 �������
    EXPECT_EQ(events[1].getDescription(), "New Year");       // 1 ������
}
�����
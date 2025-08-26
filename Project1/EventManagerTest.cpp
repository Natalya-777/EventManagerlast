#ifdef RUN_TESTS

#include <gtest/gtest.h>
#include "EventManager.h"

class EventManagerTest : public ::testing::Test {
protected:
    EventManager eventManager;
public:
    EventManagerTest() : eventManager("C:\\TOP\\EventManager\\events.txt") {}
};

TEST_F(EventManagerTest, ValidDate) {
    std::string dateStr = "25.12.2023";
    auto result = eventManager.parseDate(dateStr);
    EXPECT_EQ(result, std::chrono::sys_days{ std::chrono::year{2023} / 12 / 25 });
}

TEST_F(EventManagerTest, InvalidFormat) {
    std::string dateStr = "25-12-2023";
    EXPECT_THROW(eventManager.parseDate(dateStr), std::runtime_error);
}

TEST_F(EventManagerTest, InvalidDate) {
    std::string dateStr = "31.02.2023";
    EXPECT_THROW(eventManager.parseDate(dateStr), std::runtime_error);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#endif
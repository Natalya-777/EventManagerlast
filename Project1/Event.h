
#pragma once

#include <string>
#include <chrono>

class Event {
public:
    Event(std::chrono::sys_days date, const std::string& description);

    std::chrono::sys_days getDate() const;
    std::string getDescription() const;

    bool operator<(const Event& other) const;

private:
    std::chrono::sys_days date_;
    std::string description_;
};

#pragma once
#include <string>

class ITimeSettable {
public:
    virtual ~ITimeSettable() = default;
    virtual bool setTime(int day, int month, int year, int hour, int minute, const std::string& ampm) = 0;
};

class IDateSettable {
public:
    virtual ~IDateSettable() = default;
    virtual bool setDate(int day, int month, int year, bool preserve_time = true) = 0;
};

class IDisplayUpdatable {
public:
    virtual ~IDisplayUpdatable() = default;
    virtual void updateDisplay(const std::string& text) = 0;
};

class IPickerSelectionHandler {
public:
    virtual ~IPickerSelectionHandler() = default;
    virtual void onSelectionChanged() = 0;
};

#pragma once
#include "interfaces.h"
#include <gtkmm.h>

class SystemTimeManager : public ITimeSettable {
public:
    bool setTime(int day, int month, int year, int hour, int minute, const std::string& ampm) override;
};

class SystemDateManager : public IDateSettable {
public:
    bool setDate(int day, int month, int year, bool preserve_time = true) override;
};

class NavigationManager {
private:
    Gtk::Stack* stack_container_;

public:
    NavigationManager(Gtk::Stack* stack_container);
    void navigateTo(const std::string& page_name);
};

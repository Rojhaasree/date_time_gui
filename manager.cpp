#include "managers.h"
#include <sys/time.h>
#include <iostream>
#include <ctime>

bool SystemTimeManager::setTime(int day, int month, int year, int hour, int minute, const std::string& ampm) {
    struct tm timeinfo = {};
    
    // Convert to 24-hour format
    int hour24 = hour;
    if (ampm == "PM" && hour != 12) {
        hour24 += 12;
    } else if (ampm == "AM" && hour == 12) {
        hour24 = 0;
    }
    
    // Set the time structure
    timeinfo.tm_year = year - 1900;  // Years since 1900
    timeinfo.tm_mon = month - 1;     // Months since January (0-11)
    timeinfo.tm_mday = day;
    timeinfo.tm_hour = hour24;
    timeinfo.tm_min = minute;
    timeinfo.tm_sec = 0;
    timeinfo.tm_isdst = -1;          // Let system determine DST
    
    // Convert to time_t
    time_t new_time = mktime(&timeinfo);
    if (new_time == -1) {
        std::cerr << "Error: Invalid time specified" << std::endl;
        return false;
    }
    
    // Set system time (Linux)
    struct timeval tv;
    tv.tv_sec = new_time;
    tv.tv_usec = 0;
    
    if (settimeofday(&tv, nullptr) == 0) {
        std::cout << "System time set successfully to: " 
                  << day << "/" << month << "/" << year 
                  << " " << hour << ":" << minute << " " << ampm << std::endl;
        return true;
    } else {
        std::cerr << "Error: Failed to set system time. Run with sudo privileges." << std::endl;
        return false;
    }
}

bool SystemDateManager::setDate(int day, int month, int year, bool preserve_time) {
    struct tm timeinfo = {};
    
    if (preserve_time) {
        // Get current time to preserve it
        time_t now = time(0);
        struct tm* current_time = localtime(&now);
        
        // Preserve current time, update only date
        timeinfo.tm_hour = current_time->tm_hour;
        timeinfo.tm_min = current_time->tm_min;
        timeinfo.tm_sec = current_time->tm_sec;
    } else {
        // Set time to midnight if not preserving
        timeinfo.tm_hour = 0;
        timeinfo.tm_min = 0;
        timeinfo.tm_sec = 0;
    }
    
    // Set the date components
    timeinfo.tm_year = year - 1900;  // Years since 1900
    timeinfo.tm_mon = month - 1;     // Months since January (0-11)
    timeinfo.tm_mday = day;
    timeinfo.tm_isdst = -1;          // Let system determine DST
    
    // Convert to time_t
    time_t new_time = mktime(&timeinfo);
    if (new_time == -1) {
        std::cerr << "Error: Invalid date specified" << std::endl;
        return false;
    }
    
    // Set system time (Linux)
    struct timeval tv;
    tv.tv_sec = new_time;
    tv.tv_usec = 0;
    
    if (settimeofday(&tv, nullptr) == 0) {
        std::cout << "System date set successfully to: " 
                  << day << "/" << month << "/" << year << std::endl;
        return true;
    } else {
        std::cerr << "Error: Failed to set system date. Run with sudo privileges." << std::endl;
        return false;
    }
}

NavigationManager::NavigationManager(Gtk::Stack* stack_container) : stack_container_(stack_container) {}

void NavigationManager::navigateTo(const std::string& page_name) {
    if (stack_container_) {
        stack_container_->set_visible_child(page_name);
    }
}

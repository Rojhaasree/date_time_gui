#pragma once
#include <vector>
#include <string>

std::vector<std::string> generateNumbers(int start, int end);
inline std::vector<std::string> generateNumbers(int start, int end) {
    std::vector<std::string> items;
    for (int i = start; i <= end; ++i)
        items.push_back(std::to_string(i));
    return items;
}

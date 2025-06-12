#include <iostream>
#include <unordered_map>
#include <vector>

#include "../include/Settings.h"

void Settings::configureDefault() {
    ARR = 1.f;
    DAS = 8.f;
    DCD = 1.f;
    SDF = std::numeric_limits<float>::max();

    keyBindings = {
        {"LEFT",    {260, 106}},         // left arrow key, j key
        {"RIGHT",   {261, 108}},         // right arrow key, l key
        {"ROTATE_CW",  {120}},           // x key
        {"ROTATE_CCW", {122}},           // z key
        {"HOLD",    {99, 259, 105}},          // c key, up arrow, i key
        {"SOFT_DROP", {258, 107}},       // down arrow, k key
        {"HARD_DROP", {32}},             // space key
        {"QUIT",    {113, 27}},          // q key, esc
        {"RESTART", {114, 92}}           // r key, '\'
    };

    std::cout << "Default settings configured:\n"
              << "ARR: " << ARR << "\n"
              << "DAS: " << DAS << "\n"
              << "DCD: " << DCD << "\n"
              << "SDF: " << SDF << "\n";
    for (const auto& [action, keys] : keyBindings) {
        std::cout << "Key " << action << ": ";
        for (size_t i = 0; i < keys.size(); ++i) {
            std::cout << keys[i];
            if (i != keys.size() - 1) std::cout << ", ";
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
}

void Settings::configure() {
    std::cout << "Configure settings:\n";
    std::cout << "Enter auto repeat rate (default 1.0): ";
    std::cin >> ARR;
    std::cout << "Enter delayed auto shift (default 8.0): ";
    std::cin >> DAS;
    std::cout << "Enter das cut delay (default 1.0): ";
    std::cin >> DCD;
    std::cout << "Enter soft drop factor (default max float): ";
    std::cin >> SDF;
    
    std::cout << "Settings configured:\n"
              << "ARR: " << ARR << "\n"
              << "DAS: " << DAS << "\n"
              << "DCD: " << DCD << "\n"
              << "SDF: " << SDF << "\n";
}

std::unordered_map<std::string, std::vector<int>> Settings::getKeyBindings() const {
    return keyBindings;
}
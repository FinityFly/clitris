#include <iostream>
#include <unordered_map>
#include <vector>
#include <ncurses.h>
#include <thread>
#include <atomic>
#include <chrono>
#include <fstream>
#include <sstream>
#include <sys/stat.h>

#include "../include/Settings.h"

float Settings::ARR = 10.0f; // auto repeat rate (ms)
float Settings::DAS = 50.0f; // delayed auto shift (ms)  
float Settings::DCD = 33.0f; // das cut delay (ms)
float Settings::SDF = 1.0f; // soft drop factor (ms)

std::string Settings::mode = "zen";

std::unordered_map<std::string, std::vector<int>> Settings::keyBindings = {
    {"LEFT",    {260, 106}},         // left arrow key, j key
    {"RIGHT",   {261, 108}},         // right arrow key, l key
    {"ROTATE_CW",  {120}},           // x key
    {"ROTATE_CCW", {122}},           // z key
    {"FLIP",     {99}},              // c key
    {"HOLD",    {259, 105}},         // up arrow, i key
    {"SOFT_DROP", {258, 107}},       // down arrow, k key
    {"HARD_DROP", {32}},             // space key
    {"PAUSE",   {112}},              // p key
    {"QUIT",    {113, 27}},          // q key, esc
    {"RESTART", {114, 92}}           // r key, '\'
};

void Settings::configure() {
    clear();
    refresh();
    noecho();
    
    int term_rows, term_cols;
    getmaxyx(stdscr, term_rows, term_cols);
    
    std::vector<std::string> settingNames = {
        "LEFT", "RIGHT", "ROTATE_CW", "ROTATE_CCW", "FLIP", 
        "HOLD", "SOFT_DROP", "HARD_DROP", "QUIT", "RESTART",
        "ARR", "DAS", "DCD", "SDF"
    };
    
    std::vector<std::pair<std::string, float*>> handlingSettings = {
        {"ARR (Auto Repeat Rate)", &ARR},
        {"DAS (Delayed Auto Shift)", &DAS},
        {"DCD (DAS Cut Delay)", &DCD},
        {"SDF (Soft Drop Factor)", &SDF}
    };
    
    int currentSelection = 0;
    bool running = true;
    bool insertMode = false;
    std::string insertBuffer;
    std::vector<int> newKeys;
    
    auto check_quit = [](int ch) {
        return ch == 113 || ch == 27; // q or ESC
    };
    
    WINDOW* settingswin = nullptr;
    
    auto displaySettings = [&]() {
        std::vector<std::string> lines;
        lines.push_back("Settings Configuration");
        lines.push_back("UP/DOWN: navigate, LEFT/RIGHT: adjust (+/-5)");
        lines.push_back("ENTER: edit, ESC/Q: quit" + std::string(insertMode ? " insert mode" : ""));
        lines.push_back("");
        lines.push_back("=== Key Bindings ===");
        
        for (int i = 0; i < 10; i++) {
            std::string name = settingNames[i];
            std::string prefix = (currentSelection == i) ? "> " : "  ";
            std::string keyStr = "";
            
            if (insertMode && currentSelection == i) {
                keyStr = "Press key(s)... [" + insertBuffer + "]";
            } else {
                auto it = keyBindings.find(name);
                if (it != keyBindings.end()) {
                    for (size_t j = 0; j < it->second.size(); j++) {
                        if (j > 0) keyStr += ", ";
                        int key = it->second[j];
                        if (key == 32) keyStr += "SPACE";
                        else if (key == 27) keyStr += "ESC";
                        else if (key == 260) keyStr += "LEFT";
                        else if (key == 261) keyStr += "RIGHT";
                        else if (key == 258) keyStr += "DOWN";
                        else if (key == 259) keyStr += "UP";
                        else if (key >= 32 && key <= 126) keyStr += (char)key;
                        else keyStr += std::to_string(key);
                    }
                }
            }
            lines.push_back(prefix + name + ": " + keyStr);
        }
        
        lines.push_back("");
        lines.push_back("=== Handling Settings ===");
        for (int i = 0; i < 4; i++) {
            int settingIndex = i + 10;
            std::string prefix = (currentSelection == settingIndex) ? "> " : "  ";
            std::string valueStr;
            
            if (insertMode && currentSelection == settingIndex) {
                valueStr = "Enter value: [" + insertBuffer + "]";
            } else {
                valueStr = std::to_string(*(handlingSettings[i].second));
                // remove trailing zeros
                valueStr.erase(valueStr.find_last_not_of('0') + 1, std::string::npos);
                valueStr.erase(valueStr.find_last_not_of('.') + 1, std::string::npos);
            }
            lines.push_back(prefix + handlingSettings[i].first + ": " + valueStr);
        }
        
        int max_width = 0;
        for (const auto& line : lines) {
            max_width = std::max(max_width, (int)line.size());
        }
        int box_width = max_width + 4;
        int box_height = (int)lines.size() + 2;
        
        int starty = (term_rows - box_height) / 2;
        int startx = (term_cols - box_width) / 2;
        
        clear();
        refresh();
        
        if (settingswin) {
            delwin(settingswin);
        }
        settingswin = newwin(box_height, box_width, starty, startx);
        
        box(settingswin, 0, 0);
        
        for (size_t i = 0; i < lines.size(); i++) {
            mvwprintw(settingswin, (int)i + 1, 2, "%s", lines[i].c_str());
        }
        
        wrefresh(settingswin);
        refresh();
    };
    
    while (running) {
        displaySettings();
        
        int ch = getch();
        if (insertMode) {
            if (check_quit(ch)) {
                insertMode = false;
                insertBuffer.clear();
                newKeys.clear();
            } else if (ch == '\n' || ch == KEY_ENTER) {
                if (currentSelection < 10) {
                    // keybind settings
                    if (!newKeys.empty()) {
                        keyBindings[settingNames[currentSelection]] = newKeys;
                    }
                } else {
                    // handling settings
                    if (!insertBuffer.empty()) {
                        float value = std::stof(insertBuffer);
                        if (value >= 0.1f && value <= 99.0f) {
                            *(handlingSettings[currentSelection - 10].second) = value;
                        }
                    }
                }
                insertMode = false;
                insertBuffer.clear();
                newKeys.clear();
            } else if (ch == KEY_BACKSPACE || ch == 127) {
                if (!insertBuffer.empty()) {
                    insertBuffer.pop_back();
                }
                if (!newKeys.empty()) {
                    newKeys.pop_back();
                }
            } else if (currentSelection < 10) {
                // keybind settings
                newKeys.push_back(ch);
                if (ch == 32) insertBuffer += "SPACE ";
                else if (ch == 27) insertBuffer += "ESC ";
                else if (ch == 260) insertBuffer += "LEFT ";
                else if (ch == 261) insertBuffer += "RIGHT ";
                else if (ch == 258) insertBuffer += "DOWN ";
                else if (ch == 259) insertBuffer += "UP ";
                else if (ch >= 32 && ch <= 126) insertBuffer += (char)ch;
                else insertBuffer += std::to_string(ch) + " ";
            } else {
                // handling settings
                if ((ch >= '0' && ch <= '9') || ch == '.') {
                    insertBuffer += (char)ch;
                }
            }
        } else {
            if (check_quit(ch)) {
                running = false;
            } else if (ch == KEY_UP) {
                currentSelection = (currentSelection - 1 + settingNames.size()) % settingNames.size();
            } else if (ch == KEY_DOWN) {
                currentSelection = (currentSelection + 1) % settingNames.size();
            } else if (ch == KEY_LEFT && currentSelection >= 10) {
                // decrease handling by 10
                int handlingIndex = currentSelection - 10;
                float* setting = handlingSettings[handlingIndex].second;
                *setting = std::max(0.1f, *setting - 5.0f);
            } else if (ch == KEY_RIGHT && currentSelection >= 10) {
                // increase handling by 10
                int handlingIndex = currentSelection - 10;
                float* setting = handlingSettings[handlingIndex].second;
                *setting = std::min(99.0f, *setting + 5.0f);
            } else if (ch == '\n' || ch == KEY_ENTER) {
                insertMode = true;
                insertBuffer.clear();
                newKeys.clear();
            }
        }
    }
    
    if (settingswin) {
        delwin(settingswin);
    }

    saveConfig();

    clear();
    refresh();
}

std::unordered_map<std::string, std::vector<int>> Settings::getKeyBindings() {
    return keyBindings;
}

std::string Settings::getUserDataPath() {
#ifdef _WIN32
    const char* appdata = std::getenv("APPDATA");
    if (appdata) return std::string(appdata) + "\\clitris\\";
#elif __APPLE__ || __MACH__
    const char* home = std::getenv("HOME");
    if (home) return std::string(home) + "/Library/Application Support/clitris/";
#else
    const char* home = std::getenv("HOME");
    if (home) return std::string(home) + "/.config/clitris/";
#endif
    return "./";
}

void Settings::saveConfig() {
    std::string path = getUserDataPath();
#ifdef _WIN32
    _mkdir(path.c_str());
#else
    mkdir(path.c_str(), 0755);
#endif
    std::ofstream file(path + "settings.bin", std::ios::binary);
    if (!file) return;

    file.write(reinterpret_cast<const char*>(&ARR), sizeof(ARR));
    file.write(reinterpret_cast<const char*>(&DAS), sizeof(DAS));
    file.write(reinterpret_cast<const char*>(&DCD), sizeof(DCD));
    file.write(reinterpret_cast<const char*>(&SDF), sizeof(SDF));

    const char* actions[] = {"LEFT", "RIGHT", "ROTATE_CW", "ROTATE_CCW", "FLIP", "HOLD", "SOFT_DROP", "HARD_DROP", "PAUSE", "QUIT", "RESTART"};
    for (int i = 0; i < 11; ++i) {
        const auto& keys = keyBindings[actions[i]];
        int size = keys.size();
        file.write(reinterpret_cast<const char*>(&size), sizeof(size));
        for (int k : keys) file.write(reinterpret_cast<const char*>(&k), sizeof(k));
    }
    file.close();
}

void Settings::loadConfig() {
    std::string path = getUserDataPath();
    std::ifstream file(path + "settings.bin", std::ios::binary);
    if (!file) {
        saveConfig();
        return;
    }
    file.read(reinterpret_cast<char*>(&ARR), sizeof(ARR));
    file.read(reinterpret_cast<char*>(&DAS), sizeof(DAS));
    file.read(reinterpret_cast<char*>(&DCD), sizeof(DCD));
    file.read(reinterpret_cast<char*>(&SDF), sizeof(SDF));
    const char* actions[] = {"LEFT", "RIGHT", "ROTATE_CW", "ROTATE_CCW", "FLIP", "HOLD", "SOFT_DROP", "HARD_DROP", "PAUSE", "QUIT", "RESTART"};
    for (int i = 0; i < 11; ++i) {
        int size = 0;
        file.read(reinterpret_cast<char*>(&size), sizeof(size));
        std::vector<int> keys(size);
        for (int j = 0; j < size; ++j) file.read(reinterpret_cast<char*>(&keys[j]), sizeof(int));
        keyBindings[actions[i]] = keys;
    }
    file.close();
}
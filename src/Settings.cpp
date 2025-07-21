#include <iostream>
#include <unordered_map>
#include <vector>
#include <curses.h>
#include <thread>
#include <atomic>
#include <chrono>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <cstdint>
#include <stdexcept>

#ifdef _WIN32
    #include <winsock2.h>
    #include <direct.h>
    #define htole32(x) (x)
    #define le32toh(x) (x)
#elif defined(__APPLE__)
    #include <machine/endian.h>
    #include <libkern/OSByteOrder.h>
    #define htole32(x) OSSwapHostToLittleInt32(x)
    #define le32toh(x) OSSwapLittleToHostInt32(x)
#else
    #include <endian.h>
#endif

#include "../include/Settings.h"

// Configuration constants
static constexpr uint32_t SETTINGS_FORMAT_VERSION = 1;
static constexpr uint32_t MAX_KEYBIND_SIZE = 100;
static_assert(sizeof(uint32_t) == 4, "uint32_t must be 4 bytes");

float Settings::ARR = 10.0f; // auto repeat rate (ms)
float Settings::DAS = 50.0f; // delayed auto shift (ms)  
float Settings::DCD = 33.0f; // das cut delay (ms)
float Settings::SDF = 1.0f; // soft drop factor (ms)

std::string Settings::mode = "zen";

char Settings::tetrominoCharacter = '.';

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
        "ARR", "DAS", "DCD", "SDF", "TETROMINO_CHAR"
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
    
    // calculate max label width for key-value alignment
    int max_label_width = 0;
    for (int i = 0; i < 10; i++) {
        int len = (int)settingNames[i].size();
        if (len > max_label_width) max_label_width = len;
    }
    for (int i = 0; i < 4; i++) {
        int len = (int)handlingSettings[i].first.size();
        if (len > max_label_width) max_label_width = len;
    }
    int min_value_width = 18;
    int max_value_width = min_value_width;
    // precompute max value width for all keybinds/handling/character
    for (int i = 0; i < 10; i++) {
        auto it = keyBindings.find(settingNames[i]);
        std::string keyStr;
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
        if ((int)keyStr.size() > max_value_width) max_value_width = keyStr.size();
    }
    for (int i = 0; i < 4; i++) {
        std::string valueStr = std::to_string(*(handlingSettings[i].second));
        valueStr.erase(valueStr.find_last_not_of('0') + 1, std::string::npos);
        valueStr.erase(valueStr.find_last_not_of('.') + 1, std::string::npos);
        if ((int)valueStr.size() > max_value_width) max_value_width = valueStr.size();
    }
    // tetromino character setting
    std::string charStr = std::string(1, tetrominoCharacter);
    if ((int)charStr.size() > max_value_width) max_value_width = charStr.size();
    // add extra space for input buffer
    max_value_width = std::max(max_value_width, 24);
    int box_width = max_label_width + max_value_width + 16;
    int box_height = 11 + 10 + 4 + 3; // 10 keybinds, 4 handling, 1 character + headers
    int starty = (term_rows - box_height) / 2;
    int startx = (term_cols - box_width) / 2;

    WINDOW* outerwin = nullptr;
    WINDOW* settingswin = nullptr;

    auto displaySettings = [&]() {
        if (outerwin) delwin(outerwin);
        if (settingswin) delwin(settingswin);
        outerwin = newwin(box_height + 2, box_width + 4, starty - 1, startx - 2);
        settingswin = newwin(box_height, box_width, starty, startx);
        wattron(outerwin, A_DIM);
        box(outerwin, 0, 0);
        wattroff(outerwin, A_DIM);
        wrefresh(outerwin);
        wattron(settingswin, A_BOLD);
        box(settingswin, 0, 0);
        wattroff(settingswin, A_BOLD);

        int row = 1;
        // title
        std::string title = "SETTINGS CONFIGURATION";
        int title_x = (box_width - (int)title.size()) / 2;
        mvwprintw(settingswin, row++, title_x, "%s", title.c_str());

        // separator line
        wattron(settingswin, A_DIM);
        for (int i = 2; i < box_width - 2; ++i) {
            mvwprintw(settingswin, row, i, "=");
        }
        wattroff(settingswin, A_DIM);
        row++;

        // instructions
        std::string instr1 = "UP/DOWN: navigate, LEFT/RIGHT: adjust (+/-5)";
        std::string instr2 = "ENTER: edit, ESC/Q: quit" + std::string(insertMode ? " insert mode" : "");
        int instr1_x = (box_width - (int)instr1.size()) / 2;
        int instr2_x = (box_width - (int)instr2.size()) / 2;
        mvwprintw(settingswin, row++, instr1_x, "%s", instr1.c_str());
        mvwprintw(settingswin, row++, instr2_x, "%s", instr2.c_str());
        row++;

        // key bind settings
        std::string kb_header = "=== Key Bindings ===";
        int kb_header_x = (box_width - (int)kb_header.size()) / 2;
        mvwprintw(settingswin, row++, kb_header_x, "%s", kb_header.c_str());

        for (int i = 0; i < 10; i++) {
            std::string label = settingNames[i];
            std::string value;
            if (insertMode && currentSelection == i) {
                value = "Bind to: [" + insertBuffer + "]";
            } else {
                auto it = keyBindings.find(label);
                if (it != keyBindings.end()) {
                    for (size_t j = 0; j < it->second.size(); j++) {
                        if (j > 0) value += ", ";
                        int key = it->second[j];
                        if (key == 32) value += "SPACE";
                        else if (key == 27) value += "ESC";
                        else if (key == 260) value += "LEFT";
                        else if (key == 261) value += "RIGHT";
                        else if (key == 258) value += "DOWN";
                        else if (key == 259) value += "UP";
                        else if (key >= 32 && key <= 126) value += (char)key;
                        else value += std::to_string(key);
                    }
                }
            }
            std::string prefix = (currentSelection == i) ? "> " : "  ";
            int label_x = 2 + (int)prefix.size();
            int value_x = box_width - 2 - max_value_width;

            std::string shown_value = value;
            if ((int)shown_value.size() > max_value_width) shown_value = shown_value.substr(0, max_value_width);
            else shown_value.append(max_value_width - shown_value.size(), ' ');
            mvwprintw(settingswin, row, 2, "%s", prefix.c_str());
            wattron(settingswin, (currentSelection == i) ? A_BOLD : A_NORMAL);
            mvwprintw(settingswin, row, label_x, "%s", label.c_str());
            wattroff(settingswin, (currentSelection == i) ? A_BOLD : A_NORMAL);
            mvwprintw(settingswin, row, value_x, "%s", shown_value.c_str());
            row++;
        }
        row++;
        // handling settings
        std::string hs_header = "=== Handling Settings ===";
        int hs_header_x = (box_width - (int)hs_header.size()) / 2;
        mvwprintw(settingswin, row++, hs_header_x, "%s", hs_header.c_str());
        for (int i = 0; i < 4; i++) {
            int settingIndex = i + 10;
            std::string label = handlingSettings[i].first;
            std::string value;
            if (insertMode && currentSelection == settingIndex) {
                value = "Enter value: [" + insertBuffer + "]";
            } else {
                value = std::to_string(*(handlingSettings[i].second));
                value.erase(value.find_last_not_of('0') + 1, std::string::npos);
                value.erase(value.find_last_not_of('.') + 1, std::string::npos);
            }
            std::string prefix = (currentSelection == settingIndex) ? "> " : "  ";
            int label_x = 2 + (int)prefix.size();
            int value_x = box_width - 2 - max_value_width;

            std::string shown_value = value;
            if ((int)shown_value.size() > max_value_width) shown_value = shown_value.substr(0, max_value_width);
            else shown_value.append(max_value_width - shown_value.size(), ' ');
            mvwprintw(settingswin, row, 2, "%s", prefix.c_str());
            wattron(settingswin, (currentSelection == settingIndex) ? A_BOLD : A_NORMAL);
            mvwprintw(settingswin, row, label_x, "%s", label.c_str());
            wattroff(settingswin, (currentSelection == settingIndex) ? A_BOLD : A_NORMAL);
            mvwprintw(settingswin, row, value_x, "%s", shown_value.c_str());
            row++;
        }
        row++;
        // display settings
        std::string ds_header = "=== Display Settings ===";
        int ds_header_x = (box_width - (int)ds_header.size()) / 2;
        mvwprintw(settingswin, row++, ds_header_x, "%s", ds_header.c_str());
        
        // tetromino character setting
        int charSettingIndex = 14; // after 10 keybinds + 4 handling
        std::string charLabel = "Tetromino Character";
        std::string charValue;
        if (insertMode && currentSelection == charSettingIndex) {
            charValue = "Enter char: [" + insertBuffer + "]";
        } else {
            charValue = std::string(1, tetrominoCharacter);
        }
        std::string charPrefix = (currentSelection == charSettingIndex) ? "> " : "  ";
        int char_label_x = 2 + (int)charPrefix.size();
        int char_value_x = box_width - 2 - max_value_width;

        std::string shown_char_value = charValue;
        if ((int)shown_char_value.size() > max_value_width) shown_char_value = shown_char_value.substr(0, max_value_width);
        else shown_char_value.append(max_value_width - shown_char_value.size(), ' ');
        mvwprintw(settingswin, row, 2, "%s", charPrefix.c_str());
        wattron(settingswin, (currentSelection == charSettingIndex) ? A_BOLD : A_NORMAL);
        mvwprintw(settingswin, row, char_label_x, "%s", charLabel.c_str());
        wattroff(settingswin, (currentSelection == charSettingIndex) ? A_BOLD : A_NORMAL);
        mvwprintw(settingswin, row, char_value_x, "%s", shown_char_value.c_str());
        row++;
        
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
                        // check if any of the newKeys are existing keybinds
                        bool exists = false;
                        for (int key : newKeys) {
                            for (auto keybind : keyBindings) {
                                if (keybind.first == settingNames[currentSelection]) continue;
                                for (int keycode : keybind.second) {
                                    if (key == keycode) {
                                        exists = true;
                                        break;
                                    }
                                }
                                if (exists) break;
                            }
                            if (exists) break;
                        }
                        if (!exists) keyBindings[settingNames[currentSelection]] = newKeys;
                    }
                } else if (currentSelection < 14) {
                    // handling settings
                    if (!insertBuffer.empty()) {
                        float value = std::stof(insertBuffer);
                        if (value >= 0.1f && value <= 99.0f) {
                            *(handlingSettings[currentSelection - 10].second) = value;
                        }
                    }
                } else {
                    // tetromino character setting
                    if (!insertBuffer.empty()) {
                        tetrominoCharacter = insertBuffer[0];
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
            } else if (currentSelection < 14) {
                // handling settings
                if ((ch >= '0' && ch <= '9') || ch == '.') {
                    insertBuffer += (char)ch;
                }
            } else {
                // tetromino character setting
                if (ch >= 32 && ch <= 126) {
                    insertBuffer = std::string(1, (char)ch);
                }
            }
        } else {
            if (check_quit(ch)) {
                running = false;
            } else if (ch == KEY_UP) {
                currentSelection = (currentSelection - 1 + settingNames.size()) % settingNames.size();
            } else if (ch == KEY_DOWN) {
                currentSelection = (currentSelection + 1) % settingNames.size();
            } else if (ch == KEY_LEFT && currentSelection >= 10 && currentSelection < 14) {
                // decrease handling by 5
                int handlingIndex = currentSelection - 10;
                float* setting = handlingSettings[handlingIndex].second;
                *setting = std::max(0.1f, *setting - 5.0f);
            } else if (ch == KEY_RIGHT && currentSelection >= 10 && currentSelection < 14) {
                // increase handling by 5
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
    if (outerwin) {
        delwin(outerwin);
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

    // write format version
    uint32_t version = htole32(SETTINGS_FORMAT_VERSION);
    file.write(reinterpret_cast<const char*>(&version), sizeof(version));

    file.write(reinterpret_cast<const char*>(&ARR), sizeof(ARR));
    file.write(reinterpret_cast<const char*>(&DAS), sizeof(DAS));
    file.write(reinterpret_cast<const char*>(&DCD), sizeof(DCD));
    file.write(reinterpret_cast<const char*>(&SDF), sizeof(SDF));
    const char* actions[] = {"LEFT", "RIGHT", "ROTATE_CW", "ROTATE_CCW", "FLIP", "HOLD", "SOFT_DROP", "HARD_DROP", "PAUSE", "QUIT", "RESTART"};
    for (int i = 0; i < 11; ++i) {
        const auto& keys = keyBindings[actions[i]];
        uint32_t size = htole32(static_cast<uint32_t>(keys.size()));
        file.write(reinterpret_cast<const char*>(&size), sizeof(size));
        for (int k : keys) {
            uint32_t key = htole32(static_cast<uint32_t>(k));
            file.write(reinterpret_cast<const char*>(&key), sizeof(key));
        }
    }

    file.write(reinterpret_cast<const char*>(&tetrominoCharacter), sizeof(tetrominoCharacter));

    file.close();
}

void Settings::loadConfig() {
    std::string path = getUserDataPath();
    std::ifstream file(path + "settings.bin", std::ios::binary);
    if (!file) {
        saveConfig();
        return;
    }

    try {
        // read and validate format version
        uint32_t version = 0;
        file.read(reinterpret_cast<char*>(&version), sizeof(version));
        if (file.gcount() != sizeof(version)) {
            throw std::runtime_error("Failed to read format version - file truncated");
        }
        version = le32toh(version);
        
        if (version != SETTINGS_FORMAT_VERSION) {
            throw std::runtime_error("Unsupported format version " + std::to_string(version) + 
                                   " (expected " + std::to_string(SETTINGS_FORMAT_VERSION) + ")");
        }

        // read handling settings with validation
        file.read(reinterpret_cast<char*>(&ARR), sizeof(ARR));
        if (file.gcount() != sizeof(ARR)) {
            throw std::runtime_error("Failed to read ARR setting");
        }
        
        file.read(reinterpret_cast<char*>(&DAS), sizeof(DAS));
        if (file.gcount() != sizeof(DAS)) {
            throw std::runtime_error("Failed to read DAS setting");
        }
        
        file.read(reinterpret_cast<char*>(&DCD), sizeof(DCD));
        if (file.gcount() != sizeof(DCD)) {
            throw std::runtime_error("Failed to read DCD setting");
        }
        
        file.read(reinterpret_cast<char*>(&SDF), sizeof(SDF));
        if (file.gcount() != sizeof(SDF)) {
            throw std::runtime_error("Failed to read SDF setting");
        }

        // read key bindings with endianness conversion and validation
        const char* actions[] = {"LEFT", "RIGHT", "ROTATE_CW", "ROTATE_CCW", "FLIP", "HOLD", "SOFT_DROP", "HARD_DROP", "PAUSE", "QUIT", "RESTART"};
        for (int i = 0; i < 11; ++i) {
            uint32_t size = 0;
            file.read(reinterpret_cast<char*>(&size), sizeof(size));
            if (file.gcount() != sizeof(size)) {
                throw std::runtime_error("Failed to read key binding size for " + std::string(actions[i]));
            }
            size = le32toh(size);
            
            // sanity check to prevent excessive memory allocation
            if (size > MAX_KEYBIND_SIZE) {
                throw std::runtime_error("Invalid key binding size " + std::to_string(size) + " for " + actions[i]);
            }
            
            std::vector<int> keys(size);
            for (uint32_t j = 0; j < size; ++j) {
                uint32_t key = 0;
                file.read(reinterpret_cast<char*>(&key), sizeof(key));
                if (file.gcount() != sizeof(key)) {
                    throw std::runtime_error("Failed to read key " + std::to_string(j) + " for " + actions[i]);
                }
                keys[j] = static_cast<int>(le32toh(key));
            }
            keyBindings[actions[i]] = keys;
        }

        // Read tetromino character
        file.read(reinterpret_cast<char*>(&tetrominoCharacter), sizeof(tetrominoCharacter));
        if (file.gcount() != sizeof(tetrominoCharacter)) {
            throw std::runtime_error("Failed to read tetromino character");
        }

    } catch (const std::exception& e) {
        std::cerr << "Error loading settings: " << e.what() << std::endl;
        std::cerr << "Using default settings and regenerating config file" << std::endl;
        file.close();
        saveConfig();
        return;
    }

    file.close();
}

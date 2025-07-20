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

std::unordered_map<char, TetrominoStyle> Settings::tetrominoStyles = {
    {'I', {'x', "cyan"}},
    {'J', {'x', "blue"}},
    {'L', {'x', "orange"}},
    {'O', {'x', "yellow"}},
    {'S', {'x', "green"}},
    {'T', {'x', "purple"}},
    {'Z', {'x', "red"}}
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
    // precompute max value width for all keybinds/handling
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
    // add extra space for input buffer
    max_value_width = std::max(max_value_width, 24);
    int box_width = max_label_width + max_value_width + 16;
    int box_height = 11 + 10 + 4; // 10 keybinds, 4 handling
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

TetrominoStyle Settings::getTetrominoStyle(char type) {
    auto it = tetrominoStyles.find(type);
    if (it != tetrominoStyles.end()) {
        return it->second;
    }
    return {' ', "white"};
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

    for (const auto& [tetromino, style] : tetrominoStyles) {
        char tetromino_char = tetromino;
        file.write(reinterpret_cast<const char*>(&tetromino_char), sizeof(tetromino_char));
        file.write(reinterpret_cast<const char*>(&style.character), sizeof(style.character));
        int color_len = style.color.size();
        file.write(reinterpret_cast<const char*>(&color_len), sizeof(color_len));
        file.write(style.color.c_str(), color_len);
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

    tetrominoStyles.clear();
    while (file.peek() != EOF) {
        char tetromino_char;
        TetrominoStyle style;
        if (file.read(reinterpret_cast<char*>(&tetromino_char), sizeof(tetromino_char))) {
            file.read(reinterpret_cast<char*>(&style.character), sizeof(style.character));
            int color_len;
            file.read(reinterpret_cast<char*>(&color_len), sizeof(color_len));
            if (color_len > 0 && color_len < 100) { // sanity check
                char color_buf[100];
                file.read(color_buf, color_len);
                style.color = std::string(color_buf, color_len);
                tetrominoStyles[tetromino_char] = style;
            }
        } else {
            break;
        }
    }

    file.close();
}

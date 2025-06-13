#include <iostream>
#include <unordered_map>
#include <vector>
#include <ncurses.h>

#include "../include/Settings.h"

void Settings::configureDefault() {
    ARR = 1.f;
    DAS = 8.f;
    DCD = 1.f;
    SDF = 99.f;

    keyBindings = {
        {"LEFT",    {260, 106}},         // left arrow key, j key
        {"RIGHT",   {261, 108}},         // right arrow key, l key
        {"ROTATE_CW",  {120}},           // x key
        {"ROTATE_CCW", {122}},           // z key
        {"HOLD",    {99, 259, 105}},     // c key, up arrow, i key
        {"SOFT_DROP", {258, 107}},       // down arrow, k key
        {"HARD_DROP", {32}},             // space key
        {"QUIT",    {113, 27}},          // q key, esc
        {"RESTART", {114, 92}}           // r key, '\'
    };
}

void Settings::configure() {
    erase();
    refresh();

    echo();
    mvprintw(0, 0, "Configure settings (press 'q' or ESC to quit):");

    auto check_quit = [this](int ch) {
        auto it = keyBindings.find("QUIT");
        if (it != keyBindings.end()) {
            for (int key : it->second) {
                if (ch == key) return true;
            }
        }
        return false;
    };

    std::pair<std::string, float*> prompts[] = {
        {"Enter ARR (0.1-10.0)", &ARR},
        {"Enter DAS (0.1-10.0)", &DAS},
        {"Enter DCD (0.1-10.0)", &DCD},
        {"Enter SDF (0.1-10.0, 99.0)", &SDF}
    };

    for (int i = 0; i < 4; ++i) {
        mvprintw(i + 2, 0, "%s [%.1f]: ", prompts[i].first.c_str(), *(prompts[i].second));
        refresh();
        char input[8];
        getnstr(input, 7);
        if (check_quit(input[0])) { noecho(); return; }
        sscanf(input, "%f", prompts[i].second);
    }

    mvprintw(7, 0, "Settings configured:");
    mvprintw(8, 0, "ARR: %.2f", ARR);
    mvprintw(9, 0, "DAS: %.2f", DAS);
    mvprintw(10, 0, "DCD: %.2f", DCD);
    mvprintw(11, 0, "SDF: %.2f", SDF);
    noecho();
    refresh();
    int ch = getch();
    if (check_quit(ch)) return;
}

std::unordered_map<std::string, std::vector<int>> Settings::getKeyBindings() const {
    return keyBindings;
}

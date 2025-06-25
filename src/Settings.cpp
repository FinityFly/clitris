#include <iostream>
#include <unordered_map>
#include <vector>
#include <ncurses.h>
#include <thread>
#include <atomic>
#include <chrono>

#include "../include/Settings.h"

float Settings::ARR = 33.0f; // auto repeat rate (ms)
float Settings::DAS = 100.0f; // delayed auto shift (ms)  
float Settings::DCD = 0.0f; // das cut delay (ms)
float Settings::SDF = 1.0f; // soft drop factor (ms)

std::unordered_map<std::string, std::vector<int>> Settings::keyBindings = {
    {"LEFT",    {260, 106}},         // left arrow key, j key
    {"RIGHT",   {261, 108}},         // right arrow key, l key
    {"ROTATE_CW",  {120}},           // x key
    {"ROTATE_CCW", {122}},           // z key
    {"FLIP",     {99}},              // c key
    {"HOLD",    {259, 105}},         // up arrow, i key
    {"SOFT_DROP", {258, 107}},       // down arrow, k key
    {"HARD_DROP", {32}},             // space key
    {"QUIT",    {113, 27}},          // q key, esc
    {"RESTART", {114, 92}}           // r key, '\'
};

void Settings::configure() {
    erase();
    refresh();

    echo();
    mvprintw(0, 0, "Configure settings (press 'q' or ESC to quit):");

    std::atomic<int> lastInput{-1};
    bool running = true;

    // Input thread
    std::thread inputThread([&lastInput, &running]() {
        while (running) {
            int ch = getch();
            if (ch != ERR) {
                lastInput = ch;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });

    auto check_quit = [](int ch, const std::unordered_map<std::string, std::vector<int>>& keyBindings) {
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

    int prompt_idx = 0;
    char input[8] = {0};
    while (running && prompt_idx < 4) {
        mvprintw(prompt_idx + 2, 0, "%s [%.1f]: ", prompts[prompt_idx].first.c_str(), *(prompts[prompt_idx].second));
        refresh();
        int ch = lastInput.exchange(-1);
        if (ch != -1) {
            if (check_quit(ch, keyBindings)) { noecho(); running = false; break; }
            if (ch == '\n' || ch == KEY_ENTER) {
                sscanf(input, "%f", prompts[prompt_idx].second);
                ++prompt_idx;
                memset(input, 0, sizeof(input));
            } else if (ch == KEY_BACKSPACE || ch == 127) {
                int len = strlen(input);
                if (len > 0) input[len-1] = 0;
            } else if (ch >= 32 && ch <= 126 && strlen(input) < 7) {
                int len = strlen(input);
                input[len] = ch;
                input[len+1] = 0;
            }
            mvprintw(prompt_idx + 2, 0, "%s [%.1f]: %s ", prompts[prompt_idx].first.c_str(), *(prompts[prompt_idx].second), input);
            refresh();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    mvprintw(7, 0, "Settings configured:");
    mvprintw(8, 0, "ARR: %.2f", ARR);
    mvprintw(9, 0, "DAS: %.2f", DAS);
    mvprintw(10, 0, "DCD: %.2f", DCD);
    mvprintw(11, 0, "SDF: %.2f", SDF);
    noecho();
    refresh();
    running = false;
    inputThread.join();
    int ch = getch();
    if (check_quit(ch, keyBindings)) return;
}

std::unordered_map<std::string, std::vector<int>> Settings::getKeyBindings() {
    return keyBindings;
}

#include <ncurses.h>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <atomic>
#include <chrono>

#include "../include/Menu.h"

void Menu::display() {
    std::vector<std::string> options = {
        "[1] Start",
        "[2] Settings",
        "[3] Exit"
    };
    std::string prompt = "Select an option: ";

    int max_width = 0;
    for (const auto& line : options) {
        max_width = std::max(max_width, (int)line.size());
    }
    max_width = std::max(max_width, (int)prompt.size());
    int box_width = max_width + 6;
    int box_height = (int)options.size() + 4;

    int term_rows, term_cols;
    getmaxyx(stdscr, term_rows, term_cols);

    int starty = (term_rows - box_height) / 2;
    int startx = (term_cols - box_width) / 2;

    std::atomic<int> lastInput{-1};
    bool running = true;
    choice = -1; // Reset choice at start

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

    nodelay(stdscr, TRUE);
    erase();
    refresh();

    // Render loop
    while (running) {
        WINDOW* menuwin = newwin(box_height, box_width, starty, startx);
        box(menuwin, 0, 0);
        for (size_t i = 0; i < options.size(); ++i) {
            int opt_x = (box_width - options[i].size()) / 2;
            mvwprintw(menuwin, (int)i + 1, opt_x, "%s", options[i].c_str());
        }
        int prompt_x = (box_width - prompt.size()) / 2;
        mvwprintw(menuwin, (int)options.size() + 2, prompt_x, "%s", prompt.c_str());
        wrefresh(menuwin);
        delwin(menuwin);

        int ch = lastInput.exchange(-1);
        if (ch != -1) {
            switch (ch) {
                case '1': choice = 1; running = false; break;
                case '2': choice = 2; running = false; break;
                case '3': choice = 3; running = false; break;
                case 'q': choice = 3; running = false; break;
                default: break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    inputThread.join();
    nodelay(stdscr, FALSE);
}

int Menu::getChoice() {
    return choice;
}

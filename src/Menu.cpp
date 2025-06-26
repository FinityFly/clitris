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

    choice = -1;
    
    clear();
    refresh();
    
    WINDOW* menuwin = newwin(box_height, box_width, starty, startx);
    box(menuwin, 0, 0);
    for (size_t i = 0; i < options.size(); ++i) {
        int opt_x = (box_width - options[i].size()) / 2;
        mvwprintw(menuwin, (int)i + 1, opt_x, "%s", options[i].c_str());
    }
    int prompt_x = (box_width - prompt.size()) / 2;
    mvwprintw(menuwin, (int)options.size() + 2, prompt_x, "%s", prompt.c_str());
    wrefresh(menuwin);
    
    nodelay(stdscr, FALSE);

    while (true) {
        int ch = getch();
        switch (ch) {
            case '1': choice = 1; delwin(menuwin); return;
            case '2': choice = 2; delwin(menuwin); return;
            case '3': choice = 3; delwin(menuwin); return;
            case 'q': choice = 3; delwin(menuwin); return;
            default: break;
        }
    }
}

int Menu::getChoice() {
    return choice;
}

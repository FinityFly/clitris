#include <ncurses.h>
#include "../include/Menu.h"

void Menu::display() {
    mvprintw(5, 10, "[1] Start");
    mvprintw(6, 10, "[2] Settings");
    mvprintw(7, 10, "[3] Exit");
    mvprintw(9, 10, "Select an option: ");
    refresh();
}

int Menu::getChoice() {
    int ch = getch();
    switch (ch) {
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        default:  return -1;
    }
}

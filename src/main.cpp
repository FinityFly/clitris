#include <locale.h>
#include <csignal>
#include <ncurses.h>
#include <iostream>
#include "../include/Game.h"
#include "../include/Menu.h"
#include "../include/Settings.h"

void handle_signal(int sig) {
    (void)sig;
    endwin();
    std::_Exit(1);
}

int main() {
    setlocale(LC_ALL, "");
    initscr();

    atexit([](){ endwin(); });
    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);

    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    if (has_colors()) {
        start_color();
        // 1=White, 2=Green, 3=Yellow, 4=Blue, 5=Magenta, 6=Cyan, 7=Red
        init_pair(1, COLOR_WHITE, COLOR_WHITE);     // L
        init_pair(2, COLOR_GREEN, COLOR_GREEN);     // S
        init_pair(3, COLOR_YELLOW, COLOR_YELLOW);   // O
        init_pair(4, COLOR_BLUE, COLOR_BLUE);       // J
        init_pair(5, COLOR_MAGENTA, COLOR_MAGENTA); // T
        init_pair(6, COLOR_CYAN, COLOR_CYAN);       // I
        init_pair(7, COLOR_RED, COLOR_RED);         // Z
    }

    Settings settings;
    Menu menu;
    Game game;

    bool running = true;

    while (running) {
        clear();
        refresh();
        
        menu.display();
        int choice = menu.getChoice();

        clear();
        refresh();

        switch (choice) {
            case 1: // start game
                game.init();
                game.run(settings);
                break;
            case 2: // keybinds and handling
                settings.configure();
                break;
            case 3: // exit
                running = false;
                break;
            default:
                mvprintw(0, 0, "Invalid choice of %d. Please try again.\n", choice);
                refresh();
                getch();
        }
    }

    endwin();
    return 0;
}

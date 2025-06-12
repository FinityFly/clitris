#include <ncurses.h>
#include <iostream>
#include "../include/Game.h"
#include "../include/Menu.h"
#include "../include/Settings.h"

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    Settings settings;
    Menu menu;
    Game game;

    settings.configureDefault();
    bool running = true;

    while (running) {
        clear();
        menu.display();
        int choice = menu.getChoice();

        switch (choice) {
            case 1: // Start Game
                game.init();
                game.run(settings);
                break;
            case 2: // Settings
                settings.configure();
                break;
            case 3: // Exit
                running = false;
                break;
            default:
                mvprintw(0, 0, "Invalid choice. Please try again.\n");
                refresh();
                getch();
        }
    }

    endwin();
    return 0;
}

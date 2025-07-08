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
        // 1=White, 2=Green, 3=Yellow, 4=Blue, 5=Magenta, 6=Cyan, 7=Red, 8=Cheese
        init_pair(1, COLOR_WHITE, COLOR_WHITE);     // L
        init_pair(2, COLOR_GREEN, COLOR_GREEN);     // S
        init_pair(3, COLOR_YELLOW, COLOR_YELLOW);   // O
        init_pair(4, COLOR_BLUE, COLOR_BLUE);       // J
        init_pair(5, COLOR_MAGENTA, COLOR_MAGENTA); // T
        init_pair(6, COLOR_CYAN, COLOR_CYAN);       // I
        init_pair(7, COLOR_RED, COLOR_RED);         // Z
        init_pair(8, COLOR_WHITE, COLOR_WHITE);     // CHEESE
    }

    Settings settings;
    Menu menu;
    Game game;

    bool running = true;
    settings.loadConfig();

    while (running) {
        clear();
        refresh();
        
        menu.display(0); // main menu
        int choice = menu.getChoice();

        clear();
        refresh();

        switch (choice) {
            case 1: // sprint mode
                menu.display(1); // sprint mode sub-menu
                choice = menu.getChoice();

                clear();
                refresh();

                switch(choice) {
                    case 1: // 20L
                        settings.setMode("sprint_20l");
                        game.init();
                        game.run(settings);
                        break;
                    case 2: // 40L
                        settings.setMode("sprint_40l");
                        game.init();
                        game.run(settings);
                        break;
                    case 3: // 100L
                        settings.setMode("sprint_100l");
                        game.init();
                        game.run(settings);
                        break;
                    case -1: // back to main menu
                        continue;
                    default:
                        mvprintw(0, 0, "Invalid choice of %d. Please try again.\n", choice);
                        refresh();
                        getch();
                }
                break;
            case 2: // blitz mode
                menu.display(2); // blitz mode sub-menu
                choice = menu.getChoice();

                clear();
                refresh();

                switch(choice) {
                    case 1: // 1:00
                        settings.setMode("blitz_1min");
                        game.init();
                        game.run(settings);
                        break;
                    case 2: // 2:00
                        settings.setMode("blitz_2min");
                        game.init();
                        game.run(settings);
                        break;
                    case 3: // 4:00
                        settings.setMode("blitz_4min");
                        game.init();
                        game.run(settings);
                        break;
                    case -1: // back to main menu
                        continue;
                    default:
                        mvprintw(0, 0, "Invalid choice of %d. Please try again.\n", choice);
                        refresh();
                        getch();
                }
                break;
            case 3: // zen mode
                settings.setMode("zen");
                game.init();
                game.run(settings);
                break;
            case 4: // cheese mode
                menu.display(3); // cheese mode sub-menu
                choice = menu.getChoice();

                clear();
                refresh();

                switch(choice) {
                    case 1: // 10L
                        settings.setMode("cheese_10l");
                        game.init();
                        game.run(settings);
                        break;
                    case 2: // 18L
                        settings.setMode("cheese_18l");
                        game.init();
                        game.run(settings);
                        break;
                    case 3: // 100L
                        settings.setMode("cheese_100l");
                        game.init();
                        game.run(settings);
                        break;
                    case -1: // back to main menu
                        continue;
                    default:
                        mvprintw(0, 0, "Invalid choice of %d. Please try again.\n", choice);
                        refresh();
                        getch();
                }
                break;
            case 5: // keybinds and handling
                settings.configure();
                break;
            case -1: // exit
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

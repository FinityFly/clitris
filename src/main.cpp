#include <iostream>

#include "../include/Game.h"
#include "../include/Menu.h"
#include "../include/Settings.h"

int main() {
    Settings settings;
    Menu menu;
    Game game;

    bool running = true;

    while (running) {
        menu.display();
        int choice;
        choice = menu.getChoice();

        switch (choice) {
            case 1: // Start Game
                game.init(settings);
                game.run();
                break;
            case 2: // Settings
                settings.configure();
                break;
            case 3: // Exit
                running = false;
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}


#include <ncurses.h>
#include "../include/Game.h"
#include "../include/Settings.h"
#include "../include/Tetromino.h"
#include "../include/SRS.h"

Game::Game() : isRunning(false) {}

void Game::init() {
    isRunning = true;
    clear();
    refresh();
}

void Game::run(const Settings& settings) {
    while (isRunning) {
        handleInput(settings);
        update();
        render();
    }
}

void Game::handleInput(const Settings& settings) {
    mvprintw(0, 0, "handleInput: start                  ");
    nodelay(stdscr, false);
    int ch = getch();
    mvprintw(1, 0, "handleInput: got ch = %d ('%c')      ", ch, ch);

    static const std::unordered_map<std::string, const char*> actionMessages = {
        {"LEFT", "Left key pressed           "},
        {"RIGHT", "Right key pressed          "},
        {"ROTATE_CW", "Rotate clockwise           "},
        {"ROTATE_CCW", "Rotate counter-clockwise   "},
        {"SOFT_DROP", "Soft drop                  "},
        {"HARD_DROP", "Hard drop                  "},
        {"HOLD", "Hold piece                 "},
        {"QUIT", "Quit game                  "},
        {"RESTART", "Restart game               "}
    };

    const auto& keyBindings = settings.getKeyBindings();
    mvprintw(3, 0, "handleInput: iterating keyBindings   ");
    for (const auto& [action, keys] : keyBindings) {
        for (int key : keys) {
            mvprintw(4, 0, "Checking action: %s, key: %d        ", action.c_str(), key);
            if (ch == key) {
                mvprintw(5, 0, "Matched action: %s                  ", action.c_str());
                auto it = actionMessages.find(action);
                if (it != actionMessages.end()) {
                    mvprintw(2, 0, "%s", it->second);
                }
                if (action == "QUIT") {
                    isRunning = false;
                }
                nodelay(stdscr, FALSE);
                refresh();
                return;
            }
        }
    }
    mvprintw(5, 0, "handleInput: no key matched          ");
    refresh();
}

void Game::update() {
    // game logic
}

void Game::render() {
    // ncurses rendering logic
}
#include <ncurses.h>
#include <thread>
#include <atomic>
#include <chrono>
#include "../include/Game.h"
#include "../include/Settings.h"
#include "../include/Tetromino.h"
#include "../include/SRS.h"

Game::Game() : isRunning(false), lastInput(-1) {}

void Game::init() {
    isRunning = true;
    erase();
    refresh();
}

void Game::run(const Settings& settings) {
    isRunning = true;
    lastInput = -1;
    nodelay(stdscr, TRUE); // Non-blocking input

    // Input thread
    std::thread inputThread([&]() {
        while (isRunning) {
            int ch = getch();
            if (ch != ERR) {
                lastInput = ch;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });

    // Main game loop
    while (isRunning) {
        handleInput(settings); // Now checks lastInput, not getch()
        update();
        render();
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }

    inputThread.join();
}

void Game::handleInput(const Settings& settings) {
    int ch = lastInput.exchange(-1); // Get and reset
    if (ch == -1) return; // No input

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
    for (const auto& [action, keys] : keyBindings) {
        for (int key : keys) {
            if (ch == key) {
                auto it = actionMessages.find(action);
                if (it != actionMessages.end()) {
                    mvprintw(2, 0, "%s", it->second);
                }
                if (action == "QUIT") {
                    isRunning = false;
                }
                refresh();
                return;
            }
        }
    }
    refresh();
}

void Game::update() {
    // game logic
}

void Game::render() {
    int term_rows, term_cols;
    getmaxyx(stdscr, term_rows, term_cols);

    int board_width = 10;
    int board_height = 40;

    int cell_width = 2;
    int win_height = board_height + 2;
    int win_width = board_width * cell_width + 2; 

    int start_y = (term_rows - board_height) / 2;
    int start_x = (term_cols - win_width) / 2;

    // new window for the board
    WINDOW* boardwin = newwin(win_height, win_width, start_y, start_x);
    box(boardwin, 0, 0);

    // Draw board cells and faded grid, with alternating pairs of periods and semicolons in columns
    for (int y = 0; y < board_height; ++y) {
        for (int x = 0; x < board_width; ++x) {
            int draw_x = x * cell_width + 1;
            if (board.size() > y && board[y].size() > x && board[y][x] != 0) {
                wattron(boardwin, COLOR_PAIR(board[y][x]));
                // Draw left half
                mvwaddch(boardwin, y + 1, draw_x, ' ');
                // Draw right half with a marker for the "middle"
                mvwaddch(boardwin, y + 1, draw_x + 1, ACS_BULLET);
                wattroff(boardwin, COLOR_PAIR(board[y][x]));
            } else {
                wattron(boardwin, A_DIM);
                char blank = (x % 2 == 0) ? ':' : '.';
                for (int i = 0; i < cell_width; ++i) {
                    mvwaddch(boardwin, y + 1, draw_x + i, blank);
                }
                wattroff(boardwin, A_DIM);
            }
        }
    }

    wrefresh(boardwin);
    delwin(boardwin);
}
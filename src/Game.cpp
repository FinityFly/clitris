#include <ncurses.h>
#include <thread>
#include <atomic>
#include <chrono>
#include <ctime>
#include <fstream>
#include <queue>

#include "../include/Game.h"
#include "../include/GameUtils.h"
#include "../include/Settings.h"
#include "../include/Tetromino.h"
#include "../include/SRS.h"

Game::Game()
    : isRunning(false),
      board(40, std::vector<int>(10, 0)),
      fout("tetris_log.txt", std::ios::trunc) {
    // Generate two bags and concatenate
    auto bag1 = GameUtils::generateBag();
    auto bag2 = GameUtils::generateBag();
    bag.reserve(bag1.size() + bag2.size());
    bag.insert(bag.end(), bag1.begin(), bag1.end());
    bag.insert(bag.end(), bag2.begin(), bag2.end());
    fout << "Bag generated with " << bag.size() << " pieces.\n";
    currentPiece = Tetromino(bag.back());
    bag.pop_back();
    holdPiece = NULL;
    holdAvailable = true;
    fallDelay = 500; // ms
    lastFallTime = std::chrono::steady_clock::now();
    std::time_t now = std::time(nullptr);
    fout << "Game initialized at " << std::ctime(&now);
}

void Game::init() {
    isRunning = true;
    erase();
    refresh();
}

void Game::run(const Settings& settings) {
    isRunning = true;
    nodelay(stdscr, TRUE);
    leftHeld = false;
    rightHeld = false;
    softDropHeld = false;
    lastDirection = 0;
    // Timing for ARR/DAS/SDF
    float arr = settings.getARR(); // auto repeat rate (ms)
    float das = settings.getDAS(); // delayed auto shift (ms)
    float dcd = settings.getDCD(); // das cut delay (ms)
    float sdf = settings.getSDF(); // soft drop factor (ms)
    auto lastLeft = std::chrono::steady_clock::now();
    auto lastRight = std::chrono::steady_clock::now();
    auto lastSoftDrop = std::chrono::steady_clock::now();
    bool leftInitial = true, rightInitial = true, softDropInitial = true;
    bool leftDCD = false, rightDCD = false;
    std::unordered_set<int> heldKeys;
    while (isRunning) {
        int ch;
        bool sawLeft = false, sawRight = false, sawSoftDrop = false;
        while ((ch = getch()) != ERR) {
            const auto& keyBindings = settings.getKeyBindings();
            for (const auto& [action, keys] : keyBindings) {
                for (int key : keys) {
                    if (ch == key) {
                        if (action == "LEFT") {
                            if (!leftHeld) {
                                // From neutral, use DAS
                                leftInitial = true;
                                leftDCD = false;
                                lastLeft = std::chrono::steady_clock::now();
                            } else if (lastDirection != -1) {
                                // Switching direction, use DCD
                                leftDCD = true;
                                leftInitial = false;
                                lastLeft = std::chrono::steady_clock::now();
                            }
                            leftHeld = true;
                            sawLeft = true;
                            lastDirection = -1;
                        } else if (action == "RIGHT") {
                            if (!rightHeld) {
                                rightInitial = true;
                                rightDCD = false;
                                lastRight = std::chrono::steady_clock::now();
                            } else if (lastDirection != 1) {
                                rightDCD = true;
                                rightInitial = false;
                                lastRight = std::chrono::steady_clock::now();
                            }
                            rightHeld = true;
                            sawRight = true;
                            lastDirection = 1;
                        } else if (action == "SOFT_DROP") {
                            softDropHeld = true;
                            sawSoftDrop = true;
                            lastSoftDrop = std::chrono::steady_clock::now();
                        } else {
                            handleInput(settings, ch);
                        }
                    }
                }
            }
        }

        if (!sawLeft) { leftHeld = false; leftInitial = true; leftDCD = false; }
        if (!sawRight) { rightHeld = false; rightInitial = true; rightDCD = false; }
        if (!sawSoftDrop) { softDropHeld = false; softDropInitial = true; }
        
        auto now = std::chrono::steady_clock::now();
        auto leftDuration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastLeft).count();
        auto rightDuration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastRight).count();
        auto softDropDuration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSoftDrop).count();

        // Left movement logic
        if (leftHeld && (!rightHeld || lastDirection == -1)) {
            float delay = leftInitial ? das : (leftDCD ? dcd : arr);
            fout << "Left duration: " << leftDuration << ", delay: " << delay << "\n";
            if (leftInitial || leftDCD || leftDuration >= delay) { // only using leftInitial and leftDCD
                fout << "MOVE LEFT: " << leftInitial << " " << leftDCD << " " << (leftDuration >= delay) << "\n";
                Tetromino moved = currentPiece;
                moved.moveLeft();
                if (GameUtils::canPlace(moved, board)) {
                    currentPiece.moveLeft();
                }
                lastLeft = now;
                if (leftInitial) leftInitial = false;
                if (leftDCD) leftDCD = false;
            }
        } else {
            leftInitial = true;
            leftDCD = false;
        }

        // Right movement logic
        if (rightHeld && (!leftHeld || lastDirection == 1)) {
            float delay = rightInitial ? das : (rightDCD ? dcd : arr);
            fout << "Right duration: " << rightDuration << ", delay: " << delay << "\n";
            if (rightInitial || rightDCD || rightDuration >= delay) { // only using rightInitial and rightDCD
                fout << "MOVE RIGHT: " << rightInitial << " " << rightDCD << " " << (rightDuration >= delay) << "\n";
                Tetromino moved = currentPiece;
                moved.moveRight();
                if (GameUtils::canPlace(moved, board)) {
                    currentPiece.moveRight();
                }
                lastRight = now;
                if (rightInitial) rightInitial = false;
                if (rightDCD) rightDCD = false;
            }
        } else {
            rightInitial = true;
            rightDCD = false;
        }

        // Soft drop logic
        if (softDropHeld) {
            if (softDropInitial || softDropDuration >= sdf) { // only using softDropInitial
                Tetromino moved = currentPiece;
                moved.setY(currentPiece.getY() + 1);
                if (GameUtils::canPlace(moved, board)) {
                    currentPiece.setY(currentPiece.getY() + 1);
                }
                lastSoftDrop = now;
                softDropInitial = false;
            }
        } else {
            softDropInitial = true;
        }

        update();
        render();
        // std::this_thread::sleep_for(std::chrono::milliseconds(8)); // ~120 FPS
    }
}

void Game::handleInput(const Settings& settings, int ch) {
    // Remove keyHeld logic for rotation/hold
    const auto& keyBindings = settings.getKeyBindings();
    for (const auto& [action, keys] : keyBindings) {
        for (int key : keys) {
            if (ch == key) {
                if (action == "ROTATE_CW") {
                    Tetromino rotated = currentPiece;
                    rotated.rotateCW(board);
                    if (GameUtils::canPlace(rotated, board)) {
                        currentPiece.rotateCW(board);
                    }
                } else if (action == "ROTATE_CCW") {
                    Tetromino rotated = currentPiece;
                    rotated.rotateCCW(board);
                    if (GameUtils::canPlace(rotated, board)) {
                        currentPiece.rotateCCW(board);
                    }
                } else if (action == "FLIP") {
                    Tetromino rotated = currentPiece;
                    rotated.rotate180(board);
                    if (GameUtils::canPlace(rotated, board)) {
                        currentPiece.rotate180(board);
                    }
                } else if (action == "HOLD") {
                    if (holdAvailable) {
                        // hold logic later
                        holdAvailable = false;
                    }
                } else if (action == "HARD_DROP") {
                    Tetromino moved = currentPiece;
                    while (GameUtils::canPlace(moved, board)) {
                        currentPiece.setY(moved.getY());
                        moved.setY(moved.getY() + 1);
                    }
                    GameUtils::placePiece(currentPiece, board);
                    if (!bag.empty()) {
                        currentPiece = bag.back();
                        bag.pop_back();
                    } else {
                        if (bag.size() <= 7) {
                            auto newBag = GameUtils::generateBag();
                            bag.insert(bag.end(), newBag.begin(), newBag.end());
                        }
                    }
                    currentPiece.setX(4);
                    currentPiece.setY(0);
                    holdAvailable = true;
                    lastFallTime = std::chrono::steady_clock::now();
                    return;
                } else if (action == "QUIT") {
                    isRunning = false;
                } else if (action == "RESTART") {
                    // Optionally implement restart logic
                }
                refresh();
            }
        }
    }
    refresh();
}

void Game::update() {
    // Drop tetromino at a controllable rate
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFallTime).count() >= fallDelay) {
        Tetromino moved = currentPiece;
        moved.setY(currentPiece.getY() + 1);
        if (GameUtils::canPlace(moved, board)) {
            currentPiece.setY(currentPiece.getY() + 1);
        } else {
            GameUtils::placePiece(currentPiece, board);
            // regenerate bag if needed
            if (bag.size() <= 7) {
                auto newBag = GameUtils::generateBag();
                bag.insert(bag.end(), newBag.begin(), newBag.end());
            }
            currentPiece = bag.back();
            bag.pop_back();
            currentPiece.setX(3); // spawn x
            currentPiece.setY(0); // spawn y
            holdAvailable = true;
        }
        lastFallTime = now;
    }
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
    WINDOW* boardwin = newwin(win_height, win_width, start_y, start_x);
    box(boardwin, 0, 0);
    // Draw board cells and faded grid
    for (int y = 0; y < board_height; ++y) {
        for (int x = 0; x < board_width; ++x) {
            int draw_x = x * cell_width + 1;
            char val = board[y][x]; // color value
            if (val != 0) {
                wattron(boardwin, COLOR_PAIR(val));
                for (int i = 0; i < cell_width; ++i) {
                    mvwaddch(boardwin, y + 1, draw_x + i, ' ');
                }
                wattroff(boardwin, COLOR_PAIR(val));
            } else {
                wattron(boardwin, A_DIM);
                for (int i = 0; i < cell_width; ++i) {
                    mvwaddch(boardwin, y + 1, draw_x + i, ' ');
                }
                wattroff(boardwin, A_DIM);
            }
        }
    }
    // Draw ghost piece
    Tetromino ghost = currentPiece;
    while (true) {
        Tetromino moved = ghost;
        moved.setY(ghost.getY() + 1);
        if (GameUtils::canPlace(moved, board)) {
            ghost.setY(ghost.getY() + 1);
        } else {
            break;
        }
    }
    int ghostColor = currentPiece.getColor();
    int gpx = ghost.getX();
    int gpy = ghost.getY();
    auto gshape = ghost.getShape();
    for (int y = 0; y < (int)gshape.size(); ++y) {
        for (int x = 0; x < (int)gshape[y].size(); ++x) {
            if (gshape[y][x]) {
                int bx = gpx + x;
                int by = gpy + y;
                if (bx >= 0 && bx < 10 && by >= 0 && by < 40) {
                    int draw_x = bx * cell_width + 1;
                    wattron(boardwin, COLOR_PAIR(ghostColor) | A_DIM);
                    mvwaddch(boardwin, by + 1, draw_x, '.');
                    mvwaddch(boardwin, by + 1, draw_x + 1, '.');
                    wattroff(boardwin, COLOR_PAIR(ghostColor) | A_DIM);
                }
            }
        }
    }
    // Overlay current tetromino
    int color = currentPiece.getColor();
    int px = currentPiece.getX();
    int py = currentPiece.getY();
    auto shape = currentPiece.getShape();
    for (int y = 0; y < (int)shape.size(); ++y) {
        for (int x = 0; x < (int)shape[y].size(); ++x) {
            if (shape[y][x]) {
                int bx = px + x;
                int by = py + y;
                if (bx >= 0 && bx < 10 && by >= 0 && by < 40) {
                    int draw_x = bx * cell_width + 1;
                    wattron(boardwin, COLOR_PAIR(color));
                    mvwaddch(boardwin, by + 1, draw_x, ' ');
                    mvwaddch(boardwin, by + 1, draw_x + 1, ' ');
                    wattroff(boardwin, COLOR_PAIR(color));
                }
            }
        }
    }
    wrefresh(boardwin);
    delwin(boardwin);
}
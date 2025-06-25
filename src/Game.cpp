#include <ncurses.h>
#include <thread>
#include <atomic>
#include <chrono>
#include <ctime>
#include <fstream>
#include <queue>
#include <iomanip>

#include "../include/Game.h"
#include "../include/GameUtils.h"
#include "../include/UI.h"
#include "../include/Settings.h"
#include "../include/Tetromino.h"
#include "../include/SRS.h"

void Game::reset() {
    board.assign(40, std::vector<int>(10, 0));
    bag.clear();
    auto bag1 = GameUtils::generateBag();
    auto bag2 = GameUtils::generateBag();
    bag.reserve(bag1.size() + bag2.size());
    bag.insert(bag.end(), bag1.begin(), bag1.end());
    bag.insert(bag.end(), bag2.begin(), bag2.end());
    currentPiece = bag.front();
    bag.erase(bag.begin());
    holdPiece = NULL;
    holdAvailable = true;
    lastRotation = 0;
    statistics = {
        {"totalPieces", 0},
        {"score", 0},
        {"lines", 0},
        {"single", 0},
        {"double", 0},
        {"triple", 0},
        {"tetris", 0},
        {"tspins", 0},
        {"tspin_minis", 0},
        {"pc", 0},
        {"b2bStreak", 0},
        {"max_b2bStreak", 0},
        {"combo", 0},
        {"max_combo", 0},
    };
    lastFallTime = std::chrono::steady_clock::now();
    gameStart = std::chrono::steady_clock::now();
}

void Game::newPiece() {
    currentPiece = bag.front();
    bag.erase(bag.begin());
    if (bag.size() <= 7) {
        auto newBag = GameUtils::generateBag();
        bag.insert(bag.end(), newBag.begin(), newBag.end());
    }
    statistics["totalPieces"]++;

    if (!GameUtils::canPlace(currentPiece, board)) {
        isRunning = false;
        reset();
    }
}

Game::Game()
    : isRunning(false),
      board(40, std::vector<int>(10, 0)),
      fallDelay(500),
      gameStart(std::chrono::steady_clock::now()) {
    reset();
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
                                // from neutral, use DAS
                                leftInitial = true;
                                leftDCD = false;
                            } else if (lastDirection != -1) {
                                // switching direction, use DCD
                                leftDCD = true;
                                leftInitial = false;
                            }
                            leftHeld = true;
                            sawLeft = true;
                            lastDirection = -1;
                        } else if (action == "RIGHT") {
                            if (!rightHeld) {
                                rightInitial = true;
                                rightDCD = false;
                            } else if (lastDirection != 1) {
                                rightDCD = true;
                                rightInitial = false;
                            }
                            rightHeld = true;
                            sawRight = true;
                            lastDirection = 1;
                        } else if (action == "SOFT_DROP") {
                            softDropHeld = true;
                            sawSoftDrop = true;
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

        // std::string softDropStr = "SoftDrop: " + std::to_string(softDropDuration) + " ms";
        // int log_y = LINES - 2;
        // int log_x = 1;
        // mvprintw(log_y, log_x, "%s", softDropStr.c_str());

        if (leftHeld && (!rightHeld || lastDirection == -1)) {
            float delay = leftInitial ? das : (leftDCD ? dcd : arr);
            if (leftInitial || leftDCD || leftDuration >= delay) { // only using leftInitial and leftDCD
                Tetromino moved = currentPiece;
                moved.moveLeft();
                if (GameUtils::canPlace(moved, board)) {
                    currentPiece.moveLeft();
                    lastLeft = now;
                }
                if (leftInitial) leftInitial = false;
                if (leftDCD) leftDCD = false;
            }
        } else {
            leftInitial = true;
            leftDCD = false;
        }

        if (rightHeld && (!leftHeld || lastDirection == 1)) {
            float delay = rightInitial ? das : (rightDCD ? dcd : arr);
            if (rightInitial || rightDCD || rightDuration >= delay) { // only using rightInitial and rightDCD
                Tetromino moved = currentPiece;
                moved.moveRight();
                if (GameUtils::canPlace(moved, board)) {
                    currentPiece.moveRight();
                    lastRight = now;
                }
                if (rightInitial) rightInitial = false;
                if (rightDCD) rightDCD = false;
            }
        } else {
            rightInitial = true;
            rightDCD = false;
        }

        if (softDropHeld) {
            // if (softDropInitial || softDropDuration >= sdf) { // only using softDropInitial
            if (softDropDuration >= sdf) {
                Tetromino moved = currentPiece;
                moved.setY(currentPiece.getY() + 1);
                if (GameUtils::canPlace(moved, board)) {
                    currentPiece.setY(currentPiece.getY() + 1);
                    lastSoftDrop = now;
                }
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
    const auto& keyBindings = settings.getKeyBindings();
    for (const auto& [action, keys] : keyBindings) {
        for (int key : keys) {
            if (ch == key) {
                if (action == "ROTATE_CW") {
                    Tetromino rotated = currentPiece;
                    rotated.rotateCW(board);
                    bool kicked = !GameUtils::canPlace(rotated, board);
                    if (GameUtils::canPlace(rotated, board)) {
                        currentPiece.rotateCW(board);
                        lastRotation = kicked ? 2 : 1;
                    } else {
                        lastRotation = 0;
                    }
                } else if (action == "ROTATE_CCW") {
                    Tetromino rotated = currentPiece;
                    rotated.rotateCCW(board);
                    bool kicked = !GameUtils::canPlace(rotated, board);
                    if (GameUtils::canPlace(rotated, board)) {
                        currentPiece.rotateCCW(board);
                        lastRotation = kicked ? 2 : 1;
                    } else {
                        lastRotation = 0;
                    }
                } else if (action == "FLIP") {
                    Tetromino rotated = currentPiece;
                    rotated.rotate180(board);
                    bool kicked = !GameUtils::canPlace(rotated, board);
                    if (GameUtils::canPlace(rotated, board)) {
                        currentPiece.rotate180(board);
                        lastRotation = kicked ? 2 : 1;
                    } else {
                        lastRotation = 0;
                    }
                } else if (action == "HOLD") {
                    if (holdAvailable) {
                        currentPiece.setRotationState(0);
                        if (holdPiece.getType() != 0) {
                            std::swap(currentPiece, holdPiece);
                        } else {
                            holdPiece = currentPiece;
                            Game::newPiece();
                        }
                        holdAvailable = false;
                        lastRotation = 0;
                    }
                } else if (action == "HARD_DROP") {
                    Tetromino moved = currentPiece;
                    while (GameUtils::canPlace(moved, board)) {
                        currentPiece.setY(moved.getY());
                        moved.setY(moved.getY() + 1);
                    }
                    GameUtils::placePiece(currentPiece, board);
                    lastRotation = 0;
                    processLineClear();

                    newPiece();

                    holdAvailable = true;
                    lastFallTime = std::chrono::steady_clock::now();
                    return;
                } else if (action == "QUIT") {
                    isRunning = false;
                    reset();
                } else if (action == "RESTART") {
                    reset();
                }
                refresh();
            }
        }
    }
    refresh();

    Tetromino moved = currentPiece;
    moved.setY(currentPiece.getY() + 1);

    // reset lock delay if piece falling
    if (!GameUtils::canPlace(moved, board)) {
        lockDelayActive = false;
    }
}

void Game::processLineClear() {
    auto clearInfo = GameUtils::checkClearConditions(currentPiece, board, lastRotation);
    int score = GameUtils::calculateScore(clearInfo, statistics["b2bStreak"], statistics["combo"]);

    statistics["score"] += score;
    statistics["lines"] += clearInfo.lines;
    if (clearInfo.lines > 0) {
        statistics["combo"] = std::max(0, statistics["combo"]) + 1;
        if (clearInfo.pc) {
            statistics["pc"]++;
        }
        if (clearInfo.tspin) {
            statistics["tspins"]++;
        } else if (clearInfo.mini) {
            statistics["tspin_minis"]++;
        } else if (clearInfo.lines == 1) {
            statistics["single"]++;
        } else if (clearInfo.lines == 2) {
            statistics["double"]++;
        } else if (clearInfo.lines == 3) {
            statistics["triple"]++;
        } else if (clearInfo.lines == 4) {
            statistics["tetris"]++;
        }
        if (clearInfo.lines == 4 || clearInfo.tspin || clearInfo.mini || clearInfo.pc) {
            statistics["b2bStreak"]++;
            statistics["max_b2bStreak"] = std::max(statistics["max_b2bStreak"], statistics["b2bStreak"]);
        } else {
            statistics["b2bStreak"] = 0;
        }
        statistics["max_combo"] = std::max(statistics["max_combo"], statistics["combo"]);
    } else {
        statistics["combo"] = 0;
    }

    generatePopup(clearInfo);

    holdAvailable = true;
    lastFallTime = std::chrono::steady_clock::now();
}

void Game::showPopup(const std::string& text, float durationSeconds) {
    popupText = text;
    popupStartTime = std::chrono::steady_clock::now();
    popupDurationSeconds = durationSeconds;
}

void Game::generatePopup(const GameUtils::ClearInfo& info) {
    std::string popupText = "";

    if (info.lines > 0 && statistics["b2bStreak"] > 1) {
        popupText += std::to_string(statistics["b2bStreak"]) + "x B2B ";
    }

    if (info.tspin || info.mini) {
        popupText += "T-SPIN ";
    }
    if (info.pc) {
        popupText += "PERFECT CLEAR ";
    } else if (info.lines == 1) {
        popupText += "SINGLE ";
    } else if (info.lines == 2) {
        popupText += "DOUBLE ";
    } else if (info.lines == 3) {
        popupText += "TRIPLE ";
    } else if (info.lines == 4) {
        popupText += "TETRIS ";
    } else if (info.mini) {
        popupText += "MINI ";
    }

    if (!popupText.empty()) popupText += "!";

    if (statistics["combo"] > 1) {
        popupText += "\n" + std::to_string(statistics["combo"]) + "x COMBO !";
    }

    showPopup(popupText);
}

void Game::update() {
    auto now = std::chrono::steady_clock::now();

    Tetromino moved = currentPiece;
    moved.setY(currentPiece.getY() + 1);

    if (!GameUtils::canPlace(moved, board)) {
        // on the ground: lock delay logic
        if (!lockDelayActive) {
            lockDelayActive = true;
            lockStartTime = now;
        }
        if (lockDelayActive && std::chrono::duration_cast<std::chrono::milliseconds>(now - lockStartTime).count() >= lockDelayMs) {
            GameUtils::placePiece(currentPiece, board);
            processLineClear();
            newPiece();
            holdAvailable = true;
            lastRotation = 0;
            lockDelayActive = false;
            lastFallTime = now;
        }
    } else {
        // not on the ground: gravity logic
        lockDelayActive = false;
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFallTime).count() >= fallDelay) {
            currentPiece.setY(currentPiece.getY() + 1);
            lastFallTime = now;
        }
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
    int box_width = 12;
    int box_height = 6;

    int start_y = (term_rows - board_height) / 2;
    int start_x = (term_cols - win_width) / 2;

    auto now = std::chrono::steady_clock::now();

    // board window
    WINDOW* boardwin = newwin(win_height, win_width, start_y, start_x);
    box(boardwin, 0, 0);
    UI::renderBoard(boardwin, board, cell_width);
    UI::renderGhostPiece(boardwin, currentPiece, board, cell_width);
    UI::renderTetromino(boardwin, currentPiece, cell_width, false);
    wrefresh(boardwin);

    std::string scoreStr = "Score: " + std::to_string(statistics["score"]);
    int score_x = start_x + (win_width - scoreStr.size()) / 2;
    int score_y = start_y + board_height + 2;
    mvprintw(score_y, score_x, "%s", scoreStr.c_str());

    // hold window
    int hold_x = start_x - box_width - 2;
    int hold_y = start_y;
    WINDOW* holdwin = newwin(box_height, box_width, hold_y, hold_x);
    box(holdwin, 0, 0);
    mvwprintw(holdwin, 0, 2, "HOLD");
    UI::renderPieceBox(holdwin, holdPiece, cell_width);
    wrefresh(holdwin);

    // stats window
    int stats_height = 20;
    int stats_width = box_width + 2;
    int stats_y = hold_y + box_height + 1;
    int stats_x = hold_x - 2;
    WINDOW* statswin = newwin(stats_height, stats_width, stats_y, stats_x);

    double seconds = std::chrono::duration<double>(now - gameStart).count();
    UI::renderStatsWindow(statswin, statistics, seconds);
    wrefresh(statswin);

    // next window
    int next_x = start_x + win_width + 2;
    int next_y = start_y;
    int next_box_height = box_height * 4 + 2;
    int next_box_width = box_width;
    WINDOW* nextwin = newwin(next_box_height, next_box_width, next_y, next_x);
    box(nextwin, 0, 0);
    mvwprintw(nextwin, 0, 2, "NEXT");
    for (int i = 0; i < 4 && i < (int)bag.size(); ++i) {
        int piece_offset_y = 1 + i * box_height;
        WINDOW* temp = derwin(nextwin, box_height - 2, box_width - 2, piece_offset_y, 1);
        UI::renderPieceBox(temp, bag[i], cell_width);
        delwin(temp);
    }
    wrefresh(nextwin);

    // handling window
    int handling_height = 6;
    int handling_width = box_width;
    int handling_y = next_y + next_box_height + 1;
    int handling_x = next_x;
    WINDOW* handlingwin = newwin(handling_height, handling_width, handling_y, handling_x);
    UI::renderHandling(handlingwin);
    wrefresh(handlingwin);

    // popup text
    std::vector<std::string> lines;
    size_t maxLen = 0;
    size_t pos = 0, prev = 0;
    while ((pos = popupText.find('\n', prev)) != std::string::npos) {
        lines.emplace_back(popupText.substr(prev, pos - prev));
        prev = pos + 1;
    }
    lines.emplace_back(popupText.substr(prev));
    for (const auto& l : lines) maxLen = std::max(maxLen, l.size());

    int popup_y = stats_y + stats_height + 2;
    int popup_x = start_x - (int)maxLen - 1;
    if (!popupText.empty() && std::chrono::duration<double>(now - popupStartTime).count() < popupDurationSeconds) {
        for (size_t i = 0; i < lines.size(); ++i) {
            int line_x = popup_x + (int)(maxLen - lines[i].size());
            mvprintw(popup_y + (int)i, line_x, "%s", lines[i].c_str());
        }
    } else {
        for (size_t i = 0; i < lines.size(); ++i) {
            std::string spaces(start_x, ' ');
            mvprintw(popup_y + (int)i, 0, "%s", spaces.c_str());
        }
        popupText.clear();
    }

    delwin(boardwin);
    delwin(holdwin);
    delwin(nextwin);
    delwin(handlingwin);
    delwin(statswin);
}
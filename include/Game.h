#ifndef GAME_H
#define GAME_H

#include <atomic>
#include <thread>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <fstream>
#include <queue>
#include <unordered_set>

#include "Settings.h"
#include "Tetromino.h"
#include "GameUtils.h"

class Game {
public:
    Game();
    void reset();
    void newPiece();
    void init();
    void run(const Settings& settings);
    void update();
    void render();
    bool canPlace(const Tetromino& piece);
    void placePiece(const Tetromino& piece);
private:
    bool isRunning;
    std::atomic<int> lastInput{-1};
    std::vector<std::vector<int>> board;
    std::vector<Tetromino> bag;
    Tetromino currentPiece;
    Tetromino holdPiece;
    bool holdAvailable;
    bool lastRotationWasKick = false;
    std::unordered_map<std::string, int> statistics = {
        {"totalPieces", 0},
        {"score", 0},
        {"lines", 0},
        {"tspins", 0},
        {"tspin_minis", 0},
        {"b2bStreak", 0},
        {"max_b2bStreak", 0},
        {"combo", 0},
        {"max_combo", 0},
        {"perfect_clears", 0}
    };

    int fallDelay; // ms
    std::chrono::steady_clock::time_point lastFallTime;
    std::ofstream fout;
    std::queue<int> inputQueue;
    bool leftHeld = false;
    bool rightHeld = false;
    bool softDropHeld = false;
    int lastDirection = 0; // -1 for left, 1 for right, 0 for none

    void handleInput(const Settings& settings, int ch);
    void clearLines();
    void processLineClear();
    void updateStatistics(const GameUtils::ClearInfo& info);
    void showPopup(const std::string& text);
};

#endif

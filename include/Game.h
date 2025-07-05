#ifndef GAME_H
#define GAME_H

#include <atomic>
#include <thread>
#include <vector>
#include <unordered_map>
#include <chrono>
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
    bool quitPressed = false;
    bool isPaused = false;
    std::atomic<int> lastInput{-1};
    std::vector<std::vector<int>> board;
    std::vector<Tetromino> bag;
    Tetromino currentPiece;
    Tetromino holdPiece;
    bool holdAvailable;
    int lastRotation = 0; // 0 = no rotation, 1 = normal, 2 = kicked
    std::unordered_map<std::string, int> statistics = {
        {"totalPieces", 0},
        {"attack", 0},
        {"lines", 0},
        {"single", 0},
        {"double", 0},
        {"triple", 0},
        {"tetris", 0},
        {"tspins", 0},
        {"tss", 0},
        {"tsd", 0},
        {"tst", 0},
        {"tspin_minis", 0},
        {"pc", 0},
        {"b2bStreak", 0},
        {"max_b2bStreak", 0},
        {"combo", 0},
        {"max_combo", 0},
        {"score", 0}
    };
    double gameTime;
    double totalPausedDuration;

    int fallDelay; // ms
    std::chrono::steady_clock::time_point lastFallTime;
    std::queue<int> inputQueue;
    bool leftHeld = false;
    bool rightHeld = false;
    bool softDropHeld = false;
    int lastDirection = 0; // -1 for left, 1 for right, 0 for none
    std::string popupText;
    std::chrono::steady_clock::time_point popupStartTime;
    float popupDurationSeconds = 0.0;
    std::chrono::steady_clock::time_point gameStart;

    std::chrono::steady_clock::time_point lockStartTime;
    bool lockDelayActive = false;
    const int lockDelayMs = 500; // ms

    void handleInput(const Settings& settings, int ch);
    void clearLines();
    void processLineClear();
    void generatePopup(const GameUtils::ClearInfo& info);
    void showPopup(const std::string& text, float durationSeconds = 2.0);
};

#endif

#ifndef GAME_H
#define GAME_H

#include "Settings.h"
#include "Tetromino.h"
#include <atomic>
#include <thread>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <fstream>
#include <queue>
#include <unordered_set>

class Game {
public:
    Game();
    void init();
    void run(const Settings& settings);
    void handleInput(const Settings& settings, int ch);
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
    std::unordered_map<std::string, int> statistics;
    int fallDelay; // ms
    std::chrono::steady_clock::time_point lastFallTime;
    std::ofstream fout;
    std::queue<int> inputQueue;
    bool leftHeld = false;
    bool rightHeld = false;
    bool softDropHeld = false;
    int lastDirection = 0; // -1 for left, 1 for right, 0 for none
};

#endif

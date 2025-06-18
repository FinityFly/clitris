#ifndef GAME_H
#define GAME_H

#include "Settings.h"
#include <atomic>
#include <thread>

class Game {
public:
    Game();
    void init();
    void run(const Settings& settings);
    void handleInput(const Settings& settings);
    void update();
    void render();
private:
    bool isRunning;
    std::atomic<int> lastInput{-1};
    std::vector<std::vector<int>> board;
    std::vector<int> currentPiece;
    std::vector<int> bag;
    std::unordered_map<std::string, int> statistics;
};

#endif

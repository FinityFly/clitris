#ifndef GAME_H
#define GAME_H

#include "Settings.h"

class Game {
public:
    Game();
    void init(const Settings& settings);
    void run();
    void handleInput();
    void update();
    void render();
private:
    bool isRunning;
};

#endif

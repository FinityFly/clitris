#ifndef GAME_H
#define GAME_H

#include "Settings.h"

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
};

#endif

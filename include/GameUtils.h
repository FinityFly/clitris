#ifndef GAME_UTILS_H
#define GAME_UTILS_H

#include "Settings.h"
#include "Tetromino.h"
#include <atomic>
#include <thread>
#include <vector>

class GameUtils {
public:
    GameUtils() = delete;

    static std::vector<Tetromino> generateBag();
    static bool canPlace(const Tetromino& piece, const std::vector<std::vector<int>>& board);
    static void placePiece(const Tetromino& piece, std::vector<std::vector<int>>& board);
};

#endif

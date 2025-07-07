#ifndef GAME_UTILS_H
#define GAME_UTILS_H

#include <atomic>
#include <thread>
#include <vector>

#include "Settings.h"
#include "Tetromino.h"

class GameUtils {
public:
    GameUtils() = delete;
    
    struct ClearInfo {
        int lines;
        bool tspin;
        bool mini;
        bool pc;
        int cheeseCleared = 0;
    };

    static std::vector<Tetromino> generateBag();
    static bool canPlace(const Tetromino& piece, const std::vector<std::vector<int>>& board);
    static void placePiece(const Tetromino& piece, std::vector<std::vector<int>>& board);
    
    static int clearLines(std::vector<std::vector<int>>& board);
    static bool isPerfectClear(const std::vector<std::vector<int>>& board);

    static int countFilledCorners(const Tetromino& piece, const std::vector<std::vector<int>>& board);

    static ClearInfo checkClearConditions(const Tetromino& piece, std::vector<std::vector<int>>& board, int lastRotation);
    static int calculateAttack(const ClearInfo& info, int b2bStreak, int combo);
    static int calculateScore(const ClearInfo& info, int b2bStreak, int combo);
    static void generateCheeseLines(std::vector<std::vector<int>>& board, int num);
};

#endif

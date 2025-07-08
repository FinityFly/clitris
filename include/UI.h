#pragma once
#include <vector>
#include <curses.h>
#include <unordered_map>
#include <string>

#include "Tetromino.h"

class UI {
public:
    static void renderBoard(WINDOW* win, const std::vector<std::vector<int>>& board, int board_height, int board_width, int cell_width = 2);
    static void renderTetromino(WINDOW* win, const Tetromino& tetromino, int cell_width = 2, bool ghost = false);
    static void renderGhostPiece(WINDOW* win, const Tetromino& tetromino, const std::vector<std::vector<int>>& board, int cell_width = 2);
    static void renderPieceBox(WINDOW* win, const Tetromino& tetromino, int cell_width = 2);
    static void renderStatsWindow(WINDOW* win, const std::unordered_map<std::string, int>& statistics, double gameTime = 0.0);
    static void renderHandling(WINDOW* win);
    static std::string formatSeconds(double seconds);
    static void showResultsPage(const std::string& mode, const std::unordered_map<std::string, int>& statistics, double gameTime = 0.0);
    static void showPauseScreen();
};
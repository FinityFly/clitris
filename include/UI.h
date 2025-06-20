#pragma once
#include <vector>
#include <ncurses.h>
#include <unordered_map>
#include <string>

#include "Tetromino.h"

class UI {
public:
    static void renderBoard(WINDOW* win, const std::vector<std::vector<int>>& board, int cell_width = 2);
    static void renderTetromino(WINDOW* win, const Tetromino& tetromino, int cell_width = 2, bool ghost = false);
    static void renderGhostPiece(WINDOW* win, const Tetromino& tetromino, const std::vector<std::vector<int>>& board, int cell_width = 2);
    static void renderPieceBox(WINDOW* win, const Tetromino& tetromino, int cell_width = 2);
    static void renderStatsWindow(WINDOW* win, const std::unordered_map<std::string, int>& statistics, double seconds);
};
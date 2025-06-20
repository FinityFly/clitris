#pragma once
#include <vector>
#include <ncurses.h>

#include "Tetromino.h"

class UI {
public:
    static void renderBoard(WINDOW* win, const std::vector<std::vector<int>>& board, int cell_width = 2);
    static void renderTetromino(WINDOW* win, const Tetromino& tetromino, int cell_width = 2);
    static void renderPieceBox(WINDOW* win, const Tetromino& tetromino, int cell_width = 2, const char* label = nullptr);
};
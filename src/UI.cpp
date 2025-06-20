#include "../include/UI.h"
#include "../include/GameUtils.h"

void UI::renderBoard(WINDOW* win, const std::vector<std::vector<int>>& board, int cell_width) {
    int board_height = board.size();
    int board_width = board[0].size();
    for (int y = 0; y < board_height; ++y) {
        for (int x = 0; x < board_width; ++x) {
            int draw_x = x * cell_width + 1;
            char val = board[y][x];
            if (val != 0) {
                wattron(win, COLOR_PAIR(val));
                for (int i = 0; i < cell_width; ++i)
                    mvwaddch(win, y + 1, draw_x + i, ' ');
                wattroff(win, COLOR_PAIR(val));
            } else {
                wattron(win, A_DIM);
                for (int i = 0; i < cell_width; ++i)
                    mvwaddch(win, y + 1, draw_x + i, ' ');
                wattroff(win, A_DIM);
            }
        }
    }
}

void UI::renderTetromino(WINDOW* win, const Tetromino& tetromino, int cell_width, bool ghost) {
    int color = tetromino.getColor();
    int px = tetromino.getX();
    int py = tetromino.getY();
    auto shape = tetromino.getShape();
    for (int y = 0; y < (int)shape.size(); ++y) {
        for (int x = 0; x < (int)shape[y].size(); ++x) {
            if (shape[y][x]) {
                int bx = px + x;
                int by = py + y;
                int draw_x = bx * cell_width + 1;
                if (ghost) {
                    wattron(win, COLOR_PAIR(color) | A_BOLD);
                    mvwaddch(win, by + 1, draw_x, '.');
                    mvwaddch(win, by + 1, draw_x + 1, '.');
                    wattroff(win, COLOR_PAIR(color) | A_BOLD);
                } else {
                    wattron(win, COLOR_PAIR(color));
                    mvwaddch(win, by + 1, draw_x, ' ');
                    mvwaddch(win, by + 1, draw_x + 1, ' ');
                    wattroff(win, COLOR_PAIR(color));
                }
            }
        }
    }
}

void UI::renderGhostPiece(WINDOW* win, const Tetromino& tetromino, const std::vector<std::vector<int>>& board, int cell_width) {
    Tetromino ghost = tetromino;
    while (true) {
        Tetromino moved = ghost;
        moved.setY(ghost.getY() + 1);
        if (GameUtils::canPlace(moved, board)) {
            ghost.setY(ghost.getY() + 1);
        } else {
            break;
        }
    }
    renderTetromino(win, ghost, cell_width, true);
}

void UI::renderPieceBox(WINDOW* win, const Tetromino& tetromino, int cell_width) {
    if (tetromino.getType() != 0) {
        int box_height, box_width;
        getmaxyx(win, box_height, box_width);
        auto shape = tetromino.getShape();
        int shapeH = shape.size();
        int shapeW = shape[0].size();
        int offsetY = (box_height - shapeH) / 2;
        int offsetX = (box_width - shapeW * cell_width) / 2;
        int color = tetromino.getColor();
        wattron(win, COLOR_PAIR(color));
        for (int y = 0; y < shapeH; ++y) {
            for (int x = 0; x < shapeW; ++x) {
                if (shape[y][x]) {
                    for (int i = 0; i < cell_width; ++i)
                        mvwaddch(win, offsetY + y, offsetX + x * cell_width + i, ' ');
                }
            }
        }
        wattroff(win, COLOR_PAIR(color));
    }
}

void UI::renderStatsWindow(WINDOW* win, const std::unordered_map<std::string, int>& statistics, double seconds) {
    box(win, 0, 0);
    mvwprintw(win, 0, 2, "STATS");
    double pps = (seconds > 0) ? (statistics.at("totalPieces") / seconds) : 0.0;
    double sps = (seconds > 0) ? (statistics.at("score") / seconds) : 0.0;
    mvwprintw(win, 1, 1, "Score: %d", statistics.at("score"));
    mvwprintw(win, 2, 1, "Lines: %d", statistics.at("lines"));
    mvwprintw(win, 3, 1, "PPS: %.2f", pps);
    mvwprintw(win, 4, 1, "SPS: %.2f", sps);
}
#include "../include/UI.h"
#include "../include/GameUtils.h"
#include "../include/Settings.h"

void UI::renderBoard(WINDOW* win, const std::vector<std::vector<int>>& board, int board_height, int board_width, int cell_width) {
    // Only render the bottom 20 rows of the 40-row board (rows 20-39)
    int start_row = 20; // Skip the top 20 hidden rows
    for (int y = 0; y < board_height; ++y) {
        int board_y = start_row + y; // Map display row to actual board row
        for (int x = 0; x < board_width; ++x) {
            int draw_x = x * cell_width + 1;
            char val = board[board_y][x];
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
                int by = py + y - 20; // Adjust for 20-row offset (subtract hidden rows)
                int draw_x = bx * cell_width + 1;
                
                // Only draw if the piece is in the visible area (rows 20-39 of board)
                if (by >= 0 && by < 20) {
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
        int offsetY = (box_height - 3) / 2 + 1; // 3 is default shape size
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
    double apm = (seconds > 0) ? (statistics.at("attack") * 60.0 / seconds) : 0.0;

    int width;
    getmaxyx(win, std::ignore, width);

    auto print_stat = [&](int row, const char* label, const char* fmt, auto value) {
        char valbuf[32];
        snprintf(valbuf, sizeof(valbuf), fmt, value);
        int val_len = strlen(valbuf);
        mvwprintw(win, row, 1, "%s", label);
        mvwprintw(win, row, width - val_len - 1, "%s", valbuf);
    };

    int row = 1;
    print_stat(row++, "Lines:", "%d", statistics.at("lines"));
    print_stat(row++, "Attack:", "%d", statistics.at("attack"));
    print_stat(row++, "Single:", "%d", statistics.at("single"));
    print_stat(row++, "Double:", "%d", statistics.at("double"));
    print_stat(row++, "Triple:", "%d", statistics.at("triple"));
    print_stat(row++, "Tetris:", "%d", statistics.at("tetris"));
    print_stat(row++, "T-Spin:", "%d", statistics.at("tspins") + statistics.at("tspin_minis"));
    print_stat(row++, "PPS:", "%.2f", pps);
    print_stat(row++, "APM:", "%.2f", apm);
    print_stat(row++, "Time:", "%.1f", seconds);
}

void UI::renderHandling(WINDOW* win) {
    box(win, 0, 0);
    mvwprintw(win, 0, 2, "HANDLING");
    mvwprintw(win, 1, 1, "ARR: %.2f", Settings::getARR());
    mvwprintw(win, 2, 1, "DAS: %.2f", Settings::getDAS());
    mvwprintw(win, 3, 1, "DCD: %.2f", Settings::getDCD());
    mvwprintw(win, 4, 1, "SDF: %.2f", Settings::getSDF());
}
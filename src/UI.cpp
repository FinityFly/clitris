#include <unordered_map>
#include <curses.h>

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

void UI::renderStatsWindow(WINDOW* win, const std::unordered_map<std::string, int>& statistics, double gameTime) {
    box(win, 0, 0);
    mvwprintw(win, 0, 2, "STATS");
    double pps = (gameTime > 0) ? (statistics.at("totalPieces") / gameTime) : 0.0;
    double apm = (gameTime > 0) ? (statistics.at("attack") * 60.0 / gameTime) : 0.0;

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
    print_stat(row++, "Time:", "%.1f", gameTime);
}

void UI::renderHandling(WINDOW* win) {
    box(win, 0, 0);
    mvwprintw(win, 0, 2, "HANDLING");
    mvwprintw(win, 1, 1, "ARR: %.2f", Settings::getARR());
    mvwprintw(win, 2, 1, "DAS: %.2f", Settings::getDAS());
    mvwprintw(win, 3, 1, "DCD: %.2f", Settings::getDCD());
    mvwprintw(win, 4, 1, "SDF: %.2f", Settings::getSDF());
}

std::string UI::formatSeconds(double seconds) {
    int mins = static_cast<int>(seconds) / 60;
    int secs = static_cast<int>(seconds) % 60;
    int millis = static_cast<int>((seconds - static_cast<int>(seconds)) * 1000 + 0.5);
    char buf[24];
    snprintf(buf, sizeof(buf), "%d:%02d.%03d", mins, secs, millis);
    return std::string(buf);
}

void UI::showResultsPage(const std::string& mode, const std::unordered_map<std::string, int>& statistics, double gameTime) {
    clear();
    refresh();
    int term_rows, term_cols;
    getmaxyx(stdscr, term_rows, term_cols);

    static const std::vector<std::pair<std::string, std::string>> statLabels = {
        {"totalPieces", "Total Pieces"},
        {"lines", "Lines Cleared"},
        {"attack", "Lines Sent"},
        {"single", "Singles"},
        {"double", "Doubles"},
        {"triple", "Triples"},
        {"tetris", "Tetrises"},
        {"tspins", "T-Spins"},
        {"tss", "T-Spin Singles"},
        {"tsd", "T-Spin Doubles"},
        {"tst", "T-Spin Triples"},
        {"tspin_minis", "T-Spin Minis"},
        {"pc", "Perfect Clears"},
        {"max_b2bStreak", "Max B2B Streak"},
        {"max_combo", "Max Combo"},
    };

    std::vector<std::pair<std::string, std::string>> statLines;
    for (const auto& [key, label] : statLabels) {
        auto it = statistics.find(key);
        if (it != statistics.end()) {
            statLines.emplace_back(label, std::to_string(it->second));
        }
    }

    int box_width = 48;
    int box_height = std::min((int)statLines.size() + 12, term_rows - 2);
    int start_y = (term_rows - box_height) / 2;
    int start_x = (term_cols - box_width) / 2;

    WINDOW* win = newwin(box_height, box_width, start_y, start_x);
    keypad(win, TRUE);
    box(win, 0, 0);

    // PPS and APM
    double pps = (gameTime > 0) ? (statistics.at("totalPieces") / gameTime) : 0.0;
    double apm = (gameTime > 0) ? (statistics.at("attack") * 60.0 / gameTime) : 0.0;
    char pps_buf[16], apm_buf[16];
    snprintf(pps_buf, sizeof(pps_buf), "%.2f", pps);
    snprintf(apm_buf, sizeof(apm_buf), "%.2f", apm);
    statLines.insert(statLines.begin(), {"Pieces Per Second (PPS)", std::string(pps_buf)});
    statLines.insert(statLines.begin(), {"Attack Per Minute (APM)", std::string(apm_buf)});

    // mode-specific statistics
    std::string title, modeStat;
    if (mode.find("sprint_") != std::string::npos) {
        title = "SPRINT RESULTS";
        modeStat = std::string("Time: ") + formatSeconds(gameTime);
        statLines.insert(statLines.begin(), {"Score", std::to_string(statistics.at("score"))});
    } else if (mode.find("blitz_") != std::string::npos) {
        title = "BLITZ RESULTS";
        modeStat = "Score: " + std::to_string(statistics.at("score"));
        std::string gameTimeStr;
        if (mode.find("1min") != std::string::npos) gameTimeStr = "1:00.000";
        else if (mode.find("2min") != std::string::npos) gameTimeStr = "2:00.000";
        else if (mode.find("4min") != std::string::npos) gameTimeStr = "4:00.000";
        else gameTimeStr = formatSeconds(gameTime);
        statLines.insert(statLines.begin(), {"Time", gameTimeStr});
    } else if (mode == "zen") {
        title = "ZEN RESULTS";
        modeStat = "Lines: " + std::to_string(statistics.at("lines"));
        statLines.insert(statLines.begin(), {"Score", std::to_string(statistics.at("score"))});
        statLines.insert(statLines.begin(), {"Time", formatSeconds(gameTime)});
    } else if (mode.find("cheese_") != std::string::npos) {
        title = "CHEESE RESULTS";
        modeStat = std::string("Time: ") + formatSeconds(gameTime);
        statLines.insert(statLines.begin(), {"Score", std::to_string(statistics.at("score"))});
        statLines.insert(statLines.begin(), {"Cheese Cleared", std::to_string(statistics.at("cheeseCleared"))});
    } else {
        title = "GAME OVER";
        modeStat = "Mode: " + mode;
    }

    while (true) {
        // title and mode stat
        wattron(win, A_BOLD);
        int title_x = (box_width - (int)title.size()) / 2;
        mvwprintw(win, 1, title_x, "%s", title.c_str());
        wattroff(win, A_BOLD);
        if (!modeStat.empty()) {
            int stat_x = (box_width - (int)modeStat.size()) / 2;
            mvwprintw(win, 3, stat_x, "%s", modeStat.c_str());
        }

        // label value pairs
        for (int i = 0; i < static_cast<int>(statLines.size()); ++i) {
            const auto& [label, value] = statLines[i];
            mvwprintw(win, i + 5, 4, "%s", label.c_str());
            mvwprintw(win, i + 5, box_width - 4 - (int)value.size(), "%s", value.c_str());
        }
        // footer
        wattron(win, A_DIM);
        std::string instr = "[q] or [ESC] to exit";
        int instr_x = (box_width - (int)instr.size()) / 2;
        mvwprintw(win, box_height - 2, instr_x, "%s", instr.c_str());
        wattroff(win, A_DIM);
        wrefresh(win);
        int ch = wgetch(win);
        if (ch == 'q' || ch == 'Q' || ch == 27) break;
    }
    delwin(win);
    clear();
    refresh();
}

void UI::showPauseScreen() {
    int term_rows, term_cols;
    getmaxyx(stdscr, term_rows, term_cols);
    int pause_win_height = 5;
    int pause_win_width = 20;
    int start_y = (term_rows - pause_win_height) / 2;
    int start_x = (term_cols - pause_win_width) / 2;
    WINDOW* pausewin = newwin(pause_win_height, pause_win_width, start_y, start_x);
    box(pausewin, 0, 0);
    wattron(pausewin, A_BOLD);
    std::string paused = "PAUSED";
    int paused_x = (pause_win_width - paused.size()) / 2;
    mvwprintw(pausewin, 1, paused_x, "%s", paused.c_str());
    wattroff(pausewin, A_BOLD);
    std::string instr1 = "[p] to unpause";
    std::string instr2 = "[q] to quit";
    int instr1_x = (pause_win_width - instr1.size()) / 2;
    int instr2_x = (pause_win_width - instr2.size()) / 2;
    mvwprintw(pausewin, 2, instr1_x, "%s", instr1.c_str());
    mvwprintw(pausewin, 3, instr2_x, "%s", instr2.c_str());
    wrefresh(pausewin);
    delwin(pausewin);
}
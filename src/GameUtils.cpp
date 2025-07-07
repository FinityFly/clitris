#include <vector>
#include <algorithm>
#include <random>
#include <fstream>

#include "../include/GameUtils.h"
#include "../include/Tetromino.h"

std::vector<Tetromino> GameUtils::generateBag() {
    std::vector<Tetromino> bag = {
        Tetromino('I'), Tetromino('J'), Tetromino('L'),
        Tetromino('O'), Tetromino('S'), Tetromino('Z'),
        Tetromino('T')
    };
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(bag.begin(), bag.end(), g);
    return bag;
}

bool GameUtils::canPlace(const Tetromino& piece, const std::vector<std::vector<int>>& board) {
    auto shape = piece.getShape();
    int px = piece.getX();
    int py = piece.getY();
    int shapeH = shape.size();
    int shapeW = shape.empty() ? 0 : shape[0].size();
    for (int y = 0; y < shapeH; ++y) {
        for (int x = 0; x < shapeW; ++x) {
            if (shape[y][x]) {
                int bx = px + x;
                int by = py + y;
                if (bx < 0 || bx >= 10 || by < 0 || by >= 40) return false;
                if (board[by][bx] != 0) return false;
            }
        }
    }
    return true;
}

void GameUtils::placePiece(const Tetromino& piece, std::vector<std::vector<int>>& board) {
    auto shape = piece.getShape();
    int px = piece.getX();
    int py = piece.getY();
    for (int y = 0; y < (int)shape.size(); ++y) {
        for (int x = 0; x < (int)shape[y].size(); ++x) {
            if (shape[y][x]) {
                int bx = px + x;
                int by = py + y;
                if (bx < 0 || bx >= 10 || by < 0 || by >= 40) continue;
                board[by][bx] = piece.getColor();
            }
        }
    }
}

int GameUtils::clearLines(std::vector<std::vector<int>>& board) {
    int linesCleared = 0;
    for (int y = (int)board.size() - 1; y >= 0; --y) {
        bool full = std::all_of(board[y].begin(), board[y].end(), [](int cell) { return cell != 0; });
        
        if (full) {
            board.erase(board.begin() + y);
            board.insert(board.begin(), std::vector<int>(board[0].size(), 0));
            ++linesCleared;
            ++y;
        }
    }
    return linesCleared;
}

bool GameUtils::isPerfectClear(const std::vector<std::vector<int>>& board) {
    for (const auto& row : board) {
        for (int cell : row) {
            if (cell != 0) {
                return false;
            }
        }
    }
    return true;
}

int GameUtils::countFilledCorners(const Tetromino& piece, const std::vector<std::vector<int>>& board) {
    if (piece.getType() != 'T') return 0;

    int cornersFilled = 0;
    const std::vector<std::pair<int, int>> corners = {{
        {-1, -1}, {1, -1}, {-1, 1}, {1, 1}
    }};

    for (const auto& [dx, dy] : corners) {
        int x = piece.getX() + dx + 1;
        int y = piece.getY() + dy + 1;

        if (x < 0 || x >= 10 || y < 0 || y >= 40 || board[y][x] != 0) {
            cornersFilled++;
        }
    }
    
    return cornersFilled;
}

GameUtils::ClearInfo GameUtils::checkClearConditions(const Tetromino& piece, std::vector<std::vector<int>>& board, int lastRotation) {
    ClearInfo info{};
    // tspin checks
    if (piece.getType() != 'T') {
        info.tspin = false;
        info.mini = false;
    } else if (lastRotation > 0) {
        int cornersFilled = countFilledCorners(piece, board);
        info.tspin = (cornersFilled >= 3);
        info.mini = (cornersFilled == 2 && lastRotation == 2);
    }

    // identify cleared lines
    std::vector<int> clearedLines;
    for (int y = 0; y < static_cast<int>(board.size()); ++y) {
        bool isFull = std::all_of(board[y].begin(), board[y].end(), [](int cell) { return cell != 0; });
        if (isFull) {
            clearedLines.push_back(y);
        }
    }
    // count cheese among cleared lines
    int cheeseCleared = 0;
    for (int y : clearedLines) {
        int cheeseBlocks = std::count(board[y].begin(), board[y].end(), 8);
        if (cheeseBlocks == 9) {
            ++cheeseCleared;
        }
    }
    info.cheeseCleared = cheeseCleared;

    // clear lines and check for perfect clear
    info.lines = clearLines(board);
    info.pc = (info.lines > 0) && isPerfectClear(board);

    return info;
}

int GameUtils::calculateAttack(const ClearInfo& info, int b2bStreak, int combo) {
    // tetrio combo table (0-indexed)
    static const int comboTable[] = {
        0, 1, 1, 2, 2, 3, 3, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5
    };
    int linesSent = 0;
    bool b2b = (b2bStreak > 0) && (info.lines == 4 || (info.tspin && info.lines > 0) || info.mini);
    int comboIdx = std::min(combo, (int)(sizeof(comboTable)/sizeof(comboTable[0])) - 1);
    
    if (info.tspin) {
        if (info.lines == 1) linesSent = 2; // TSS
        else if (info.lines == 2) linesSent = 4; // TSD
        else if (info.lines == 3) linesSent = 6; // TST
    } else if (info.mini) {
        if (info.lines == 1) linesSent = 0; // TSM Single
        else if (info.lines == 2) linesSent = 2; // TSM Double
    } else {
        // Normal clears
        if (info.lines == 1) linesSent = 0;
        else if (info.lines == 2) linesSent = 1;
        else if (info.lines == 3) linesSent = 2;
        else if (info.lines == 4) linesSent = 4;
    }

    if (b2b) {
        if (info.lines == 4) linesSent += 1; // B2B Tetris
        else if (info.tspin && info.lines > 0) linesSent += 1; // B2B TS
        else if (info.mini && info.lines == 2) linesSent += 1; // B2B TSM Double
    }
    if (combo > 0 && info.lines > 0) {
        linesSent += comboTable[comboIdx];
    }
    if (info.pc) {
        linesSent += 10;
    }
    
    return linesSent;
}

int GameUtils::calculateScore(const ClearInfo& info, int b2bStreak, int combo) {
    int base = 0;
    if (info.mini) {
        if (info.lines == 1) base = 100; // T-Spin Mini Single
        else if (info.lines == 2) base = 200; // T-Spin Mini Double
    } else if (info.tspin) {
        if (info.lines == 1) base = 800; // T-Spin Single
        else if (info.lines == 2) base = 1200; // T-Spin Double
        else if (info.lines == 3) base = 1600; // T-Spin Triple
    } else {
        if (info.lines == 1) base = 100; // Single
        else if (info.lines == 2) base = 300; // Double
        else if (info.lines == 3) base = 500; // Triple
        else if (info.lines == 4) base = 800; // Tetris
    }

    // B2B bonus
    bool b2b = (b2bStreak > 0) && (info.lines == 4 || info.tspin);
    if (b2b && (info.lines == 4 || info.tspin)) {
        base = static_cast<int>(base * 1.5);
    }

    // Combo bonus: +50 per combo, starting from 2nd combo
    int comboBonus = (combo > 1) ? (combo - 1) * 50 : 0;

    // PC
    int pcBonus = info.pc ? 3500 : 0;

    return base + comboBonus + pcBonus;
}

void GameUtils::generateCheeseLines(std::vector<std::vector<int>>& board, int num) {
    static thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dist(0, 9);

    std::vector<std::vector<int>> cheeseRows;
    int prevHole = -1;
    for (int i = 0; i < num; ++i) {
        int hole;
        do {
            hole = dist(gen);
        } while (hole == prevHole); // ensure no duplicate hole positions
        prevHole = hole;
        std::vector<int> row(10, 8);
        row[hole] = 0;
        cheeseRows.push_back(row);
    }
    for (int i = 0; i < num; ++i) {
        board.erase(board.begin());
    }
    board.insert(board.end(), cheeseRows.begin(), cheeseRows.end());
}

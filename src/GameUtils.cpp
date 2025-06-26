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
        info.tspin = 0;
        info.mini = 0;
    } else if (piece.getType() == 'T' && lastRotation > 0) {
        int cornersFilled = countFilledCorners(piece, board);
        info.tspin = cornersFilled >= 3;
        info.mini = cornersFilled == 2 && (lastRotation == 2);
    }
    
    // lines and pc checks
    info.lines = clearLines(board);
    info.pc = (info.lines > 0) && (isPerfectClear(board));
    
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
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
            if (shape[y][x] != 0) {
                int bx = px + x;
                int by = py + y;
                if (bx < 0 || bx >= 10 || by < 0 || by >= 40 || board[by][bx])
                    return false;
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
                if (bx >= 0 && bx < 10 && by >= 0 && by < 40)
                    board[by][bx] = piece.getColor();
            }
        }
    }
}

int GameUtils::clearLines(std::vector<std::vector<int>>& board) {
    int linesCleared = 0;
    for (int y = (int)board.size() - 1; y >= 0; --y) {
        bool full = std::all_of(board[y].begin(), board[y].end(), 
                               [](int cell) { return cell != 0; });
        
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
    return std::all_of(board.begin(), board.end(), 
        [](const std::vector<int>& row) {
            return std::all_of(row.begin(), row.end(), [](int cell) { return cell == 0; });
        });
}

int GameUtils::countFilledCorners(const Tetromino& piece, const std::vector<std::vector<int>>& board) {
    if (piece.getType() != 5) return 0; // Only T pieces can T-Spin
    
    int centerX = piece.getX();
    int centerY = piece.getY();
    int cornersFilled = 0;
    
    constexpr std::array<std::pair<int, int>, 4> corners = {{
        {-1, -1}, {1, -1}, {-1, 1}, {1, 1} // Relative to center
    }};
    
    for (const auto& [dx, dy] : corners) {
        int x = centerX + dx;
        int y = centerY + dy;
        
        if (y < 0 || y >= (int)board.size() || 
            x < 0 || x >= (int)board[0].size() || 
            board[y][x] != 0) {
            cornersFilled++;
        }
    }
    
    return cornersFilled;
}

bool GameUtils::isTSpin(const Tetromino& piece, const std::vector<std::vector<int>>& board) {
    return piece.getType() == 5 && countFilledCorners(piece, board) >= 3;
}

bool GameUtils::isTSpinMini(const Tetromino& piece, const std::vector<std::vector<int>>& board, bool wallKickOccurred) {
    return piece.getType() == 5 && 
           countFilledCorners(piece, board) == 2 && 
           wallKickOccurred;
}

GameUtils::ClearInfo GameUtils::checkClearConditions(const Tetromino& piece, 
                                                  std::vector<std::vector<int>>& board,
                                                  bool wallKickOccurred) {
    ClearInfo info{};
    
    // Check for T-Spin before clearing lines
    info.tspin = isTSpin(piece, board);
    info.mini = !info.tspin && isTSpinMini(piece, board, wallKickOccurred);
    
    // Clear lines and check perfect clear
    info.lines = clearLines(board);
    info.pc = info.lines > 0 && isPerfectClear(board);
    
    return info;
}

int GameUtils::calculateScore(const ClearInfo& info, int b2bStreak, int combo) {
    int points = 0;
    
    // Base points
    if (info.tspin) {
        points = info.lines == 1 ? 800 :
                 info.lines == 2 ? 1200 :
                 info.lines == 3 ? 1600 : 400;
    } 
    else if (info.mini) {
        points = info.lines == 1 ? 200 : 100;
    } 
    else {
        points = info.lines == 1 ? 100 :
                 info.lines == 2 ? 300 :
                 info.lines == 3 ? 500 :
                 info.lines == 4 ? 800 : 0;
    }
    
    // B2B bonus
    if (b2bStreak > 0 && (info.lines == 4 || (info.tspin && info.lines > 0))) {
        points = static_cast<int>(points * (1.0 + 0.5 * std::sqrt(b2bStreak)));
    }
    
    // Combo bonus
    if (combo > 0) {
        points += combo * 50;
    }
    
    // Perfect clear bonus
    if (info.pc) {
        points += info.lines == 1 ? 1000 :
                  info.lines == 2 ? 3000 :
                  info.lines == 3 ? 5000 :
                  info.lines == 4 ? 8000 : 0;
    }
    
    return points;
}
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
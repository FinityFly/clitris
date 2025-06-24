#include "../include/SRS.h"
#include <tuple>

std::tuple<int, int, int> SRS::rotate(const Tetromino& tetromino, const std::vector<std::vector<int>>& board, bool clockwise) {
    // O-piece: no rotation, no kicks
    if (tetromino.getType() == 'O') {
        return std::make_tuple(tetromino.getX(), tetromino.getY(), 0);
    }
    int numStates = tetromino.getShapesCount();
    int oldRotation = tetromino.getRotationState();
    int nextRotation = clockwise ? (oldRotation + 1) % numStates : (oldRotation - 1 + numStates) % numStates;
    int oldX = tetromino.getX();
    int oldY = tetromino.getY();
    auto offsets = getKickOffsets(tetromino.getType(), oldRotation, clockwise);
    for (const auto& offset : offsets) {
        Tetromino testTetromino = tetromino;
        testTetromino.setRotationState(nextRotation);
        testTetromino.setX(oldX + offset.first);
        testTetromino.setY(oldY - offset.second);
        if (isValidPosition(testTetromino, board)) {
            return std::make_tuple(oldX + offset.first, oldY - offset.second, nextRotation);
        }
    }
    return std::make_tuple(oldX, oldY, oldRotation);
}

std::vector<std::pair<int, int>> SRS::getKickOffsets(char type, int rotationState, bool clockwise) {
    // SRS kick data for JLSTZ pieces
    static const std::vector<std::vector<std::pair<int, int>>> JLSTZ_CW = {
        {{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}}, // 0 -> R
        {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},     // R -> 2
        {{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}},    // 2 -> L
        {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}   // L -> 0
    };
    static const std::vector<std::vector<std::pair<int, int>>> JLSTZ_CCW = {
        {{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}},    // 0 -> L
        {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},     // R -> 0
        {{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}}, // 2 -> R
        {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}   // L -> 2
    };

    // SRS kick data for I piece
    static const std::vector<std::vector<std::pair<int, int>>> I_CW = {
        {{0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}},   // 0 -> R
        {{0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}},   // R -> 2
        {{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}},   // 2 -> L
        {{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}}    // L -> 0
    };
    static const std::vector<std::vector<std::pair<int, int>>> I_CCW = {
        {{0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}},   // 0 -> L
        {{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}},   // R -> 0
        {{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}},   // 2 -> R
        {{0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}}    // L -> 2
    };

    if (type == 'I') {
        if (clockwise) {
            return I_CW[rotationState % 4];
        } else {
            return I_CCW[rotationState % 4];
        }
    } else if (
        type == 'J' ||
        type == 'L' ||
        type == 'S' ||
        type == 'T' ||
        type == 'Z'
    ) {
        if (clockwise) {
            return JLSTZ_CW[rotationState % 4];
        } else {
            return JLSTZ_CCW[rotationState % 4];
        }
    }
    return {{0, 0}};
}

bool SRS::isValidPosition(const Tetromino& tetromino, const std::vector<std::vector<int>>& board) {
    auto shape = tetromino.getShape();
    int x = tetromino.getX();
    int y = tetromino.getY();

    for (int i = 0; i < static_cast<int>(shape.size()); ++i) {
        for (int j = 0; j < static_cast<int>(shape[i].size()); ++j) {
            if (shape[i][j] != 0) {
                int boardX = x + j;
                int boardY = y + i;

                if (boardX < 0 || boardX >= static_cast<int>(board[0].size()) || boardY < 0 || boardY >= static_cast<int>(board.size()) || board[boardY][boardX] != 0) {
                    return false;
                }
            }
        }
    }

    return true;
}

#include "../include/SRS.h"

bool SRS::canRotate(const Tetromino& tetromino, const std::vector<std::vector<int>>& board, bool clockwise) {
    Tetromino testTetromino = tetromino;
    if (clockwise) {
        testTetromino.rotateCW();
    } else {
        testTetromino.rotateCCW();
    }

    auto offsets = getKickOffsets(tetromino.getType(), tetromino.getRotationState(), clockwise);
    for (const auto& offset : offsets) {
        testTetromino.setX(tetromino.getX() + offset.first);
        testTetromino.setY(tetromino.getY() + offset.second);

        if (isValidPosition(testTetromino, board)) {
            return true;
        }
    }

    return false;
}

std::vector<std::pair<int, int>> SRS::getKickOffsets(TetrominoType type, int rotationState, bool clockwise) {
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

    if (type == TetrominoType::I) {
        if (clockwise) {
            return I_CW[rotationState % 4];
        } else {
            return I_CCW[rotationState % 4];
        }
    } else if (
        type == TetrominoType::T ||
        type == TetrominoType::J ||
        type == TetrominoType::L ||
        type == TetrominoType::S ||
        type == TetrominoType::Z
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

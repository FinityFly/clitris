#ifndef SRS_H
#define SRS_H

#include <vector>

#include "Tetromino.h"

class SRS {
public:
    static bool canRotate(const Tetromino& tetromino, const std::vector<std::vector<int>>& board, bool clockwise);

private:
    static std::vector<std::pair<int, int>> getKickOffsets(TetrominoType type, int rotationState, bool clockwise);
    static bool isValidPosition(const Tetromino& tetromino, const std::vector<std::vector<int>>& board);
};

#endif

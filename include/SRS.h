#ifndef SRS_H
#define SRS_H

#include <vector>
#include <optional>
#include <tuple>
#include "Tetromino.h"

class SRS {
public:
    // Returns std::optional<tuple<newX, newY, newRotationState>>
    static std::tuple<int, int, int> rotate(const Tetromino& tetromino, const std::vector<std::vector<int>>& board, int rotation);

private:
    static std::vector<std::pair<int, int>> getKickOffsets(char type, int rotationState, bool clockwise);
    static bool isValidPosition(const Tetromino& tetromino, const std::vector<std::vector<int>>& board);
};

#endif

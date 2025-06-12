#ifndef TETROMINO_H
#define TETROMINO_H

#include <vector>

enum class TetrominoType { I, O, T, J, L, S, Z };

class Tetromino {
public:
    Tetromino(TetrominoType type);
    void rotateCW();
    void rotateCCW();
    void moveLeft();
    void moveRight();
    void softDrop();
    void hardDrop();

    std::vector<std::vector<int>> getShape() const;
    TetrominoType getType() const;
    int getRotationState() const;
    int getX() const;
    int getY() const;

    void setX(int newX);
    void setY(int newY);
    void setRotationState(int newState);

private:
    TetrominoType type;
    std::vector<std::vector<std::vector<int>>> shapes;
    int rotationState;
    int x, y;

    void initializeShapes();
};

#endif

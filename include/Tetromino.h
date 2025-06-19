#ifndef TETROMINO_H
#define TETROMINO_H

#include <vector>

class Tetromino {
public:
    Tetromino();
    Tetromino(char type);
    Tetromino(const Tetromino& other) = default;
    Tetromino& operator=(const Tetromino& other) = default;
    void rotateCW(const std::vector<std::vector<int>>& board);
    void rotateCCW(const std::vector<std::vector<int>>& board);
    void rotate180(const std::vector<std::vector<int>>& board);
    void moveLeft();
    void moveRight();

    std::vector<std::vector<int>> getShape() const;
    char getType() const;
    int getRotationState() const;
    int getX() const;
    int getY() const;
    int getColor() const;
    int getShapesCount() const;

    void setX(int newX);
    void setY(int newY);
    void setRotationState(int newState);

private:
    char type;
    int color;
    std::vector<std::vector<std::vector<int>>> shapes;
    int rotationState;
    int x, y;

    void initializeShapes();
};

#endif

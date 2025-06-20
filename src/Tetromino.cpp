#include "../include/Tetromino.h"
#include "../include/SRS.h"
#include <tuple>

Tetromino::Tetromino() : type('I'), color(6), rotationState(0), x(3), y(0) {
    initializeShapes();
}

Tetromino::Tetromino(char type) : type(type), rotationState(0), x(3), y(0) {
    switch (type) {
        case 'I': color = 6; break; // Cyan
        case 'O': color = 3; x = 4; break; // Yellow
        case 'T': color = 5; break; // Magenta
        case 'J': color = 4; break; // Blue
        case 'L': color = 1; break; // Orange
        case 'S': color = 2; break; // Green
        case 'Z': color = 7; break; // Red
        default: color = 0; break;
    }
    initializeShapes();
}

void Tetromino::initializeShapes() {
    switch (type) {
        case 'I':
            shapes = {
                {
                    {0, 0, 0, 0},
                    {1, 1, 1, 1},
                    {0, 0, 0, 0},
                    {0, 0, 0, 0}
                },
                {
                    {0, 0, 1, 0},
                    {0, 0, 1, 0},
                    {0, 0, 1, 0},
                    {0, 0, 1, 0},
                },
                {
                    {0, 0, 0, 0},
                    {0, 0, 0, 0},
                    {1, 1, 1, 1},
                    {0, 0, 0, 0}
                },
                {
                    {0, 1, 0, 0},
                    {0, 1, 0, 0},
                    {0, 1, 0, 0},
                    {0, 1, 0, 0},
                }
            };
            break;
        case 'O':
            shapes = {{{1, 1}, {1, 1}}};
            break;
        case 'T':
            shapes = {
                {
                    {0, 1, 0},
                    {1, 1, 1},
                    {0, 0, 0}
                },
                {
                    {0, 1, 0},
                    {0, 1, 1},
                    {0, 1, 0}
                },
                {
                    {0, 0, 0},
                    {1, 1, 1},
                    {0, 1, 0}
                },
                {
                    {0, 1, 0},
                    {1, 1, 0},
                    {0, 1, 0}
                }
            };
            break;
        case 'J':
            shapes = {
                {
                    {1, 0, 0},
                    {1, 1, 1},
                    {0, 0, 0}
                },
                {
                    {0, 1, 1},
                    {0, 1, 0},
                    {0, 1, 0}
                },
                {
                    {0, 0, 0},
                    {1, 1, 1},
                    {0, 0, 1}
                },
                {
                    {0, 1, 0},
                    {0, 1, 0},
                    {1, 1, 0}
                }
            };
            break;
        case 'L':
            shapes = {
                {
                    {0, 0, 1},
                    {1, 1, 1},
                    {0, 0, 0}
                },
                {
                    {0, 1, 0},
                    {0, 1, 0},
                    {0, 1, 1}
                },
                {
                    {0, 0, 0},
                    {1, 1, 1},
                    {1, 0, 0}
                },
                {
                    {1, 1, 0},
                    {0, 1, 0},
                    {0, 1, 0}
                }
            };
            break;
        case 'S':
            shapes = {
                {
                    {0, 1, 1},
                    {1, 1, 0},
                    {0, 0, 0}
                },
                {
                    {0, 1, 0},
                    {0, 1, 1},
                    {0, 0, 1}
                },
                {
                    {0, 0, 0},
                    {0, 1, 1},
                    {1, 1, 0}
                },
                {
                    {1, 0, 0},
                    {1, 1, 0},
                    {0, 1, 0}
                }
            };
            break;
        case 'Z':
            shapes = {
                {
                    {1, 1, 0},
                    {0, 1, 1},
                    {0, 0, 0}
                },
                {
                    {0, 0, 1},
                    {0, 1, 1},
                    {0, 1, 0}
                },
                {
                    {0, 0, 0},
                    {1, 1, 0},
                    {0, 1, 1}
                },
                {
                    {0, 1, 0},
                    {1, 1, 0},
                    {1, 0, 0}
                }
            };
            break;
    }
}

void Tetromino::rotateCW(const std::vector<std::vector<int>>& board) {
    auto [newX, newY, newRotation] = SRS::rotate(*this, board, true);
    x = newX;
    y = newY;
    rotationState = newRotation;
}

void Tetromino::rotateCCW(const std::vector<std::vector<int>>& board) {
    auto [newX, newY, newRotation] = SRS::rotate(*this, board, false);
    x = newX;
    y = newY;
    rotationState = newRotation;
}

void Tetromino::rotate180(const std::vector<std::vector<int>>& board) {
    rotateCW(board);
    rotateCW(board);
}

void Tetromino::moveLeft() {
    x--;
}

void Tetromino::moveRight() {
    x++;
}

std::vector<std::vector<int>> Tetromino::getShape() const {
    return shapes[rotationState];
}

char Tetromino::getType() const {
    return type;
}

int Tetromino::getColor() const {
    return color;
}

int Tetromino::getRotationState() const {
    return rotationState;
}

int Tetromino::getX() const {
    return x;
}

int Tetromino::getY() const {
    return y;
}

void Tetromino::setX(int newX) {
    x = newX;
}

void Tetromino::setY(int newY) {
    y = newY;
}

void Tetromino::setRotationState(int newState) {
    rotationState = newState;
}

int Tetromino::getShapesCount() const {
    return static_cast<int>(shapes.size());
}
#include "../include/Tetromino.h"

Tetromino::Tetromino(TetrominoType type) : type(type), rotationState(0), x(0), y(0) {
    
}

void Tetromino::rotateCW() {

}

void Tetromino::rotateCCW() {

}

void Tetromino::moveLeft() {

}

void Tetromino::moveRight() {

}

void Tetromino::softDrop() {

}

void Tetromino::hardDrop() {

}

std::vector<std::vector<int>> Tetromino::getShape() const {
    return shapes[rotationState];
}

TetrominoType Tetromino::getType() const {
    return type;
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


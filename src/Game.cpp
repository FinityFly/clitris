#include <iostream>

#include "../include/Game.h"

Game::Game() : isRunning(false) {}

void Game::init(const Settings& settings) {
    isRunning = true;
}

void Game::run() {
    while (isRunning) {
        std::cout << "The game is running\n";
        handleInput();
        update();
        render();
    }
}

void Game::handleInput() {

}

void Game::update() {

}

void Game::render() {
	
}

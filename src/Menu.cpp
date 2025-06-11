#include <iostream>

#include "../include/Menu.h"

void Menu::display() {
    std::cout << "\t[1] Start\n";
    std::cout << "\t[2] Settings\n";
    std::cout << "\t[3] Exit\n";
}

int Menu::getChoice() {
    int choice;
    std::cin >> choice;
    return choice;
}

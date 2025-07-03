#ifndef MENU_H
#define MENU_H

class Menu {
public:
    void display(int state = 0);
    int getChoice();
private:
    int choice = -1;
};

#endif

#include <stdio.h>
#include <iostream>

#include "game/GameManager.h"

int main(int argc, char* argv[]) {
    std::cout << "Engine start\n";

    GameManager game;

    game.initialise();

    return 0;
}
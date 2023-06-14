#include <SDL2/SDL.h>
#include "headers/game.h"

#include <stdio.h>
#include <iostream>

int main(int argc, char* argv[]) {
    Game Controller;
    if(Controller.Start() != 0){
        std::cout << SDL_GetError() << "\n";
    }

    return 0;
}
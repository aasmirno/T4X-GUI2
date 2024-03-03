#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "main run\n";
    
    if(SDL_Init(SDL_INIT_VIDEO)){
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow("SDL2 Window",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          500, 500,
                                          0);


    SDL_Surface *window_surface = SDL_GetWindowSurface(window);
    SDL_UpdateWindowSurface(window);

    SDL_Delay(5000);

    return 0;
}
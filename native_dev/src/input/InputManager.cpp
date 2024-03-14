#include "input/InputManager.h"

bool InputManager::pollEvent(){
    SDL_Event event;
    while (SDL_PollEvent(&event)){
        if(event.type == SDL_QUIT){
            (*event_handler)(event);
            printf("SDL quit event\n");
        }
    }
    return false;
}
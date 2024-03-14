#include "input/InputManager.h"

bool InputManager::pollEvent(){
    SDL_Event event;
    while (SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_QUIT:
                event_handler(event);
                break;
            case SDL_MOUSEWHEEL:
                event_handler(event);
                break;
            default:
                printf("%d\n", event.type);
        }
    }
    return false;
}
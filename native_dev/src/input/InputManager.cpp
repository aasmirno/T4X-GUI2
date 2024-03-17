#include "input/InputManager.h"

bool InputManager::pollEvent()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            event_handler(event);
            break;
        case SDL_MOUSEWHEEL:
            event_handler(event);
            break;
        case SDL_KEYDOWN:
            KEYS[event.key.keysym.sym] = true;
            break;
        case SDL_KEYUP:
            KEYS[event.key.keysym.sym] = false;
            break;
        default:
            // printf("%d", event);
            break;
        }
    }
    return false;
}
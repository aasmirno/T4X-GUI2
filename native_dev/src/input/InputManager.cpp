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
            if (event.key.keysym.sym > 321){break;}
            KEYS[event.key.keysym.sym] = true;
            break;
        case SDL_KEYUP:
            if (event.key.keysym.sym > 321){break;}
            KEYS[event.key.keysym.sym] = false;
            break;
        case SDL_WINDOWEVENT:
            if(event.window.event == SDL_WINDOWEVENT_RESIZED){
                printf("resizing\n");
                event_handler(event);
            }
            break;
        default:
            //printf("%d ", event.type);
            break;
        }
    }
    return false;
}
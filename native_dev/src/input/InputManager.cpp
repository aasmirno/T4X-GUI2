#include "T4X/input/InputManager.h"

bool InputManager::pollEvent()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        Event engine_event;
        switch (event.type)
        {
        case SDL_QUIT:
            engine_event.type = E_TYPE::SDL_EVENT; engine_event.raw_event = SDL_QUIT;
            event_handler(engine_event);
            break;

        case SDL_MOUSEWHEEL:
            engine_event.type = E_TYPE::RENDER_EVENT;
            if(event.wheel.y > 0){
                engine_event.render_data = RD_DATA::MW_IN;
            } else {
                engine_event.render_data = RD_DATA::MW_OUT;
            }
            break;

        case SDL_KEYDOWN:
            if (event.key.keysym.sym <= 321){KEYS[event.key.keysym.sym] = true;}
            break;
        case SDL_KEYUP:
            if (event.key.keysym.sym <= 321){KEYS[event.key.keysym.sym] = false;}
            break;

        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                engine_event.type = E_TYPE::SCREEN_SIZE_UPDATE;
                engine_event.w = event.window.data1;
                engine_event.h = event.window.data1;
            }
            break;

        default:
            //printf("%d ", event.type);
            break;
        }

        // forward event
        ImGui_ImplSDL2_ProcessEvent(&event);
        event_handler(engine_event);
    }
    return false;
}
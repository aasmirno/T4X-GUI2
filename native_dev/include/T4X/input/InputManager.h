#pragma once
#include "T4X/input/Event.h"

#include <SDL2/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL2/SDL_opengles2.h>
#else
#include <SDL2/SDL_opengl.h>
#endif
#include <functional>

// imgui includes
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>

class InputManager
{
private:
    std::function<void(Event)> event_handler;
    bool KEYS[322]; //keyboard event state

    RENDER_KEY_STATE render_key_state;

public:
    InputManager() {}
    ~InputManager() {}

    bool* getKeys(){
        return &KEYS[0];
    }

    RENDER_KEY_STATE getRenderKeys(){
        RENDER_KEY_STATE k;
        k.W = KEYS[SDLK_w];
        k.A = KEYS[SDLK_a];
        k.S = KEYS[SDLK_s];
        k.D = KEYS[SDLK_d];
        k.Q = KEYS[SDLK_q];
        k.E = KEYS[SDLK_e];
        k.R = KEYS[SDLK_r];
        k.F = KEYS[SDLK_f];
        return k;
    }

    void set_handler(std::function<void(Event)> event_handle)
    {
        event_handler = event_handle;
    }
    bool pollEvent();
};

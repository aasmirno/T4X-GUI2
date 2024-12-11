
#pragma once
#include <SDL2/SDL.h>

enum E_TYPE
{
    NONE,
    SDL_EVENT,
    RENDER_EVENT,
    SCREEN_SIZE_UPDATE,
    GAME_EVENT
};

enum RD_DATA
{
    MW_IN,
    MW_OUT,
};

/*
    Key manager struct
*/
struct RENDER_KEY_STATE
{
    bool W = false;
    bool A = false;
    bool S = false;
    bool D = false;
    bool Q = false;
    bool E = false;
    bool R = false;
    bool F = false;
    bool P = false;
};

/*
    Engine internal event struct
*/
struct Event
{
    E_TYPE type = E_TYPE::NONE;

    // Render event data
    RD_DATA render_data;

    // raw sdl data
    SDL_EventType raw_event;

    int w = 0;
    int h = 0;
};
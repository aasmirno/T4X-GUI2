#ifndef _SURFACELOADER_H_
#define _SURFACELOADER_H_

#include <SDL2/SDL.h>

class SurfaceLoader
{
public:
    SurfaceLoader();

    static SDL_Surface *Load(SDL_Window* window, char* File);

    static bool Draw(SDL_Surface* Dest, SDL_Surface* Source, int X ,int Y);
};

#endif
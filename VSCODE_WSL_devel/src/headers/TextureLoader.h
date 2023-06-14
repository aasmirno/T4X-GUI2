#ifndef _TextureLoader_H_
#define _TextureLoader_H_

#include <SDL2/SDL.h>

class TextureLoader
{
public:
    TextureLoader();

    static SDL_Texture *Load(Uint32 WindowFormat, char *File);

    static SDL_Texture *Draw(SDL_Renderer *RenderTarget, SDL_Surface *SourceSurface, int X, int Y);
};

#endif
#include "headers/TextureLoader.h"

TextureLoader::TextureLoader()
{
}

/**
 * Load a SDL Surface from a bmp and return a texture
 * returns a formated surface
 *
 **/
SDL_Texture *TextureLoader::Load(SDL_Renderer *RenderTarget, Uint32 WindowFormat, char *File)
{
    SDL_Surface *Surface_Probe = NULL;
    SDL_Surface *Surface_Load = NULL;

    // try load file
    if ((Surface_Probe = SDL_LoadBMP(File)) == NULL)
    {
        return NULL;
    }

    // convert surface
    Surface_Load = SDL_ConvertSurfaceFormat(Surface_Probe, WindowFormat, 0);

    // free probe surface
    SDL_FreeSurface(Surface_Probe);

    return Surface_Load;
}

/**
 *  Create a Texture from a surface
 **/
SDL_Texture *TextureLoader::Draw(SDL_Renderer *RenderTarget, SDL_Surface *SourceSurface, int X, int Y)
{
    SDL_Texture *DrawTexture = NULL;

    if (RenderTarget == NULL || SourceSurface == NULL)
    {
        return NULL;
    }

    // Define the destination rectangle with the given params
    SDL_Rect destR;
    destR.x = X;
    destR.y = Y;

    // try convert surface to texture
    if ((DrawTexture = SDL_CreateTextureFromSurface(RenderTarget, SourceSurface)) == NULL)
    {
        return NULL;
    }

    return DrawTexture;
}

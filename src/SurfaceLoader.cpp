#include "headers/SurfaceLoader.h"

SurfaceLoader::SurfaceLoader(){
    
}

SDL_Surface* SurfaceLoader::Load(SDL_Window* window, char* File){
    SDL_Surface* Surface_Probe = NULL;
    SDL_Surface* Surface_Return = NULL;

    if((Surface_Probe = SDL_LoadBMP(File)) == NULL){
        return NULL;
    }

    Surface_Return = SDL_ConvertSurfaceFormat(Surface_Probe, SDL_GetWindowPixelFormat(window), 0);
    SDL_FreeSurface(Surface_Probe);

    return Surface_Return;
}

bool SurfaceLoader::Draw(SDL_Surface* Dest, SDL_Surface* Source, int X ,int Y){
    if(Dest == NULL || Source == NULL){
        return false;
    }

    SDL_Rect destR;

    destR.x = X;
    destR.y = Y;

    SDL_BlitSurface(Source, NULL, Dest, &destR);

    return true;
}

#ifndef _GAME_H_
#define _GAME_H_

#include <SDL2/SDL.h>

class Game
{
private:
    bool Running;
    
    SDL_Window *Main_Window;
    SDL_Renderer *Main_Renderer;
    SDL_Texture *Main_Texture;
    SDL_Surface *Test_Surface;
public:
    Game();

    int Start();

    bool Init();

    void HandleEvent(SDL_Event *event);

    void Loop();

    void Render();

    void Cleanup();
};

#endif
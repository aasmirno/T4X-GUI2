#include "headers/game.h"
#include "headers/TextureLoader.h"

Game::Game()
{
    Main_Window = NULL;
    Main_Renderer = NULL;
    Main_Texture = NULL;
    Test_Surface = NULL;
    Running = true;
}

int Game::Start()
{
    //initialise SDL
    if (Init() == false)
    {
        return -1;
    }

    //Enter Game Loop
    SDL_Event event;
    while (Running)
    {
        while (SDL_PollEvent(&event))
        {
            HandleEvent(&event);
        }

        Loop();
        Render();
    }

    Cleanup();
    return 0;
}

bool Game::Init()
{
    // try SDL init
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        return false;
    }

    // try SDL window creation
    if ((Main_Window = SDL_CreateWindow(
             "Main Window", // window title
             SDL_WINDOWPOS_CENTERED,
             SDL_WINDOWPOS_CENTERED,
             640,              // width, in pixels
             480,              // height, in pixels
             SDL_WINDOW_OPENGL // flags - see below
             )) == NULL)
    {
        return false;
    }

    // try SDL renderer creation
    if ((Main_Renderer = SDL_CreateRenderer(Main_Window, -1, 0)) == NULL)
    {
        return false;
    }
    SDL_RenderClear(Main_Renderer);
    SDL_SetRenderTarget(Main_Renderer, NULL);

    //test Load
    if ((Test_Surface = TextureLoader::Load(SDL_GetWindowPixelFormat(Main_Window), "build/test.bmp")) == NULL)
    {
        return false;
    }

    Main_Texture = TextureLoader::Draw(Main_Renderer, Test_Surface, 0, 0);

    return true;
}

void Game::HandleEvent(SDL_Event *event)
{
    if (event->type == SDL_QUIT)
    {
        Running = false;
    }
}

void Game::Loop()
{
}

/**
 * Render game updates using sdl:
 *   1. clear renderer
 *   2. copy changes to renderer
 *   3. present renderer
 *
 **/
void Game::Render()
{
    //clean frame buffer
    SDL_RenderClear(Main_Renderer);

    //Copy From Main Texture
    SDL_RenderCopy(Main_Renderer, Main_Texture, NULL, NULL);

    // apply any rendering
    SDL_RenderPresent(Main_Renderer);
}

void Game::Cleanup()
{
    SDL_FreeSurface(Test_Surface);
    SDL_DestroyWindow(Main_Window);
    SDL_DestroyTexture(Main_Texture);
    SDL_DestroyRenderer(Main_Renderer);
    SDL_Quit();
}
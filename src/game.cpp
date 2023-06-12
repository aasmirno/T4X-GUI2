#include "headers/game.h"
#include "headers/SurfaceLoader.h"

#include <iostream>

Game::Game()
{
    Main_Window = NULL;
    Main_Surface = NULL;
    Test_Surface = NULL;
    Running = true;
}

int Game::Start()
{
    if(Init() == false){
        std::cout << "Initialisation error\n";
        return -1;
    }

    SDL_Event event;

    while (Running)
    {
        while(SDL_PollEvent(&event)){
            HandleEvent(&event);
        }

        Loop();
        Render();
    }
    
    Cleanup();
    return 0;
}

bool Game::Init(){
    //try SDL init
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
        return false;
    }
    std::cout << "sdl init success\n";

    //try SDL window creation
    if((Main_Window = SDL_CreateWindow(
        "Main Window",                  // window title
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        640,                               // width, in pixels
        480,                               // height, in pixels
        SDL_WINDOW_OPENGL                  // flags - see below
    )) == NULL){
        return false;
    }
    std::cout << "sdl window create success\n";

    //try SDL renderer creation
    if((Main_Renderer = SDL_CreateRenderer(Main_Window, -1, 0)) == NULL){
        return false;
    }
    SDL_RenderClear(Main_Renderer);
    SDL_RenderPresent(Main_Renderer);

    //try get window surface
    if((Main_Surface = SDL_GetWindowSurface(Main_Window)) == NULL){
        return false;
    }
    std::cout << "sdl get surface success\n";

    //try load bmp
    if((Test_Surface = SurfaceLoader::Load(Main_Window, "build/test.bmp")) == NULL){
        return false;
    }
    std::cout << "sdl bmp success\n";

    return true;
}

void Game::HandleEvent(SDL_Event* event){
    if(event->type == SDL_QUIT){
        Running = false;
    }
}

void Game::Loop(){

}

void Game::Render(){
    SurfaceLoader::Draw(Main_Surface, Test_Surface, 0, 0);
    SDL_RenderClear(Main_Renderer);
    SDL_RenderPresent(Main_Renderer);
}

void Game::Cleanup(){
    SDL_FreeSurface(Main_Surface);
    SDL_FreeSurface(Test_Surface);
    SDL_DestroyWindow(Main_Window);
    SDL_Quit();
}
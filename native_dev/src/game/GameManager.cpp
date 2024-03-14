#include <game/GameManager.h>

bool GameManager::running;

bool GameManager::initialise()
{
    // initialise all sdl modules
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("ERROR: SDL failed to initialise %s\n", SDL_GetError());
        return false;
    }

    if (render_manager.initialise() == false)
    {
        printf("ERROR: render manager failed to initialise\n");
        return false;
    }
    render_manager.addRenderObject();
    render_manager.addTileObject();

    input_manager.set_handler(&handleEvent);

    running = true;
    run();
    return true;
}

bool GameManager::run(){
    while(running){
        loop();
    }
    return true;
}

void GameManager::handleEvent(SDL_Event e){
    if(e.type == SDL_QUIT){
        running = false;
    }
}

bool GameManager::loop(){
    render_manager.render();
    input_manager.pollEvent();
    return true;
}
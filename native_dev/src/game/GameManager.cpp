#include <game/GameManager.h>

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
    render_manager.addTileObject(6,6, "resources/textures/TileSet32.png", 256, 256);

    input_manager.set_handler(std::bind(&GameManager::handleEvent, this, std::placeholders::_1));

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
    } else if(e.type = SDL_MOUSEWHEEL){
        render_manager.adj_transform(e.wheel.y);
    }
}

bool GameManager::loop(){
    render_manager.render();
    input_manager.pollEvent();
    return true;
}
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
    render_manager.addTileObject();

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
        if(e.wheel.y > 0){
            render_manager.adj_transform(1.1);
        } else {
            render_manager.adj_transform(0.1);
        }
        printf("%d\n", e.wheel.y);
    }
}

bool GameManager::loop(){
    render_manager.render();
    input_manager.pollEvent();
    return true;
}
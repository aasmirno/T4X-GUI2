#include <game/GameManager.h>

bool GameManager::initialise()
{
    // initialise all sdl modules
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("ERROR: SDL failed to initialise %s\n", SDL_GetError());
        return false;
    }
    printf("SDL initialised\n");

    //try and initialise the rendering manager
    if (render_manager.initialise() == false)
    {
        printf("ERROR: render manager failed to initialise\n");
        return false;
    }
    printf("render manager initialised\n");

    unsigned x = 100;
    unsigned y = 200;
    //initialise the game map
    game_map.initialise(x, y);
    printf("gamemap initialised\n");

    //set callback for event handler
    input_manager.set_handler(std::bind(&GameManager::handleEvent, this, std::placeholders::_1));
    printf("input manager initialised\n");


    /*
        TESTING
    */
    RenderObject* test_triangle= render_manager.addRenderObject();
    if(test_triangle == nullptr){
        printf("test_triangle is fucked\n");
        return false;
    }
    printf("Test");

    TileObject* map_graphic = render_manager.addTileObject(x,y, game_map.getData(), "resources/textures/TileSet32.png", 256, 256);
    if(map_graphic == nullptr){
        printf("engine startup failed");
        return false;
    }

    //start the game
    running = true;
    run();

    //clean up
    render_manager.shutdown();

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
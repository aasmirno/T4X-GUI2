#include <game/GameManager.h>

bool GameManager::initialise()
{
    printf("Engine Start:\n");
    // initialise all sdl modules
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("ERROR: SDL failed to initialise %s\n", SDL_GetError());
        return false;
    }
    printf("    SDL initialised\n");

    //try and initialise the rendering manager
    if (render_manager.initialise(600, 600) == false)
    {
        printf("ERROR: render manager failed to initialise\n");
        return false;
    }
    printf("    render manager initialised\n");

    //initialise the game map
    game_map.initialise(map_x, map_y);
    printf("    gamemap initialised\n");

    //set callback for event handler
    input_manager.set_handler(std::bind(&GameManager::handleEvent, this, std::placeholders::_1));
    printf("    input manager initialised\n");


    /*
        TESTING
    */
    TileObject* map_graphic = render_manager.addTileObject(map_x,map_y, game_map.getData(), "resources/textures/TileSet32.png", 256, 256);
    if(map_graphic == nullptr){
        printf("engine startup failed");
        return false;
    }

    TexturedObject* logo_graphic = render_manager.addTexturedObject("resources/textures/Logo256.png", 256, 256);
    logo_graphic->update_origin(-100.0f, 100.0f);
    if(logo_graphic == nullptr){
        printf("engine startup failed");
        return false;
    }

    //start the game
    running = true;
    run();

    //clean up
    printf("Engine shutting down\n");
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
    } else if(e.type == SDL_MOUSEWHEEL){
        render_manager.adj_transform(e.wheel.y);
    } else if(e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED){
        render_manager.setScreenSize(e.window.data2, e.window.data1);
    }
}

bool GameManager::loop(){
    render_manager.render();
    input_manager.pollEvent();
    bool* keys = input_manager.getKeys();

    if(keys[SDLK_w]){
        render_manager.move_transform('w');
    }
    if(keys[SDLK_a]){
        render_manager.move_transform('a');
    }
    if(keys[SDLK_s]){
        render_manager.move_transform('s');
    }
    if(keys[SDLK_d]){
        render_manager.move_transform('d');
    }

    return true;
}
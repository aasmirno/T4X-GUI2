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

    // try and initialise the rendering manager
    if (render_manager.initialise(600, 600) == false)
    {
        printf("ERROR: render manager failed to initialise\n");
        return false;
    }
    printf("    render manager initialised\n");

    // initialise the game map
    game_map.initialise(map_x, map_y);
    printf("    gamemap initialised\n");

    // set callback for event handler
    input_manager.set_handler(std::bind(&GameManager::handleEvent, this, std::placeholders::_1));
    printf("    input manager initialised\n");

    /*
        TESTING
    */
    //TexturedObject *logo_graphic = render_manager.addTexturedObject("resources/textures/Logo256.png", 255, 54);
    //if (logo_graphic == nullptr)
    //{
    //    printf("engine startup failed");
    //    return false;
    //}

    MeshObject *test = render_manager.aMO();
    // start the game
    running = true;
    run();

    // clean up
    printf("Engine shutting down\n");
    render_manager.shutdown();

    return true;
}

bool GameManager::run()
{
    while (running)
    {
        loop();
    }
    return true;
}

void GameManager::handleEvent(SDL_Event e)
{
    if (e.type == SDL_QUIT)
    {
        running = false;
    }
    else if (e.type == SDL_MOUSEWHEEL)
    {
        render_manager.setScale(e.wheel.y);
    }
    else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED)
    {
        render_manager.setScreenSize(e.window.data2, e.window.data1);
    }
}

bool GameManager::loop()
{
    render_manager.render();
    input_manager.pollEvent();
    bool *keys = input_manager.getKeys();
    if(!keys){
        printf("Manager failed to return key array!\n");
        return false;
    }

    if (keys[SDLK_w])
    {
        render_manager.setTranslation('w');
    }
    if (keys[SDLK_a])
    {
        render_manager.setTranslation('a');
    }
    if (keys[SDLK_s])
    {
        render_manager.setTranslation('s');
    }
    if (keys[SDLK_d])
    {
        render_manager.setTranslation('d');
    }
    if (keys[SDLK_q])
    {
        render_manager.setRotation(RD_Z, RD_LEFT);
    }
    if (keys[SDLK_e])
    {
        //render_manager.setTranslation('d');
    }
    if(keys[SDLK_i]){
        render_manager.setTranslation('i');
    }
    if(keys[SDLK_j]){
        render_manager.setTranslation('j');
    }

    return true;
}
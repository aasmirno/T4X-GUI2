#include <T4X/game/GameManager.h>

bool GameManager::initialise()
{
    printf("Engine Start:\n");
    // initialise all sdl modules
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("ERROR: SDL failed to initialise %s\n", SDL_GetError());
        return false;
    }

    // try and initialise the rendering manager
    if (render_manager.initialise(600, 600) == false)
    {
        printf("ERROR: render manager failed to initialise\n");
        return false;
    }

    // initialise the game map
    game_map.initialise(400, 400);

    // set callback for event handler
    input_manager.set_handler(std::bind(&GameManager::handleEvent, this, std::placeholders::_1));

    /*
        TESTING
    */
    render_manager.addMeshObject(0);
    render_manager.setMeshData(0, game_map.getHeightMap(200), 200);
    
    if (render_manager.addTestObject() == nullptr) return false;

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

void GameManager::handleEvent(Event e)
{
    if (e.type == E_TYPE::SDL_EVENT)
    {
        if(e.raw_event == SDL_QUIT){
            running = false;
        }
    }

    if (e.type == E_TYPE::RENDER_EVENT)
    {
        render_manager.eventUpdate(e);
    }
}

bool GameManager::loop()
{
    //get events and send key updates to renderer
    input_manager.pollEvent();
    render_manager.keyUpdate(input_manager.getRenderKeys());

    //render current graphics state
    render_manager.render();

    return true;
}

void GameManager::sendKeys(){
    bool *keys = input_manager.getKeys();
    if(!keys){
        printf("[ MANAGER ERROR ] input manager failed to return array\n");
        return;
    }
}
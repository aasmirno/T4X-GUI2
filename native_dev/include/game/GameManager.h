#include "render/Renderer.h"
#include "input/InputManager.h"
#include "map/Map.h"

/*
    Overarching game managment class
        initialises and manages game modules
*/
class GameManager
{
private:
    Renderer render_manager;
    InputManager input_manager;

    /*
        Map variables
    */
    Map game_map;
    uint map_x = 100;
    uint map_y = 100;
    /*
        game state variables
    */
    bool running = false;

public:
    GameManager() {}
    ~GameManager() {}

    /*
        Initialise manager and subcomponents
    */
    bool initialise();

    bool run();

    /*
        Run one game iteration
    */
    bool loop();

    /*
         Handle event function
    */
    void handleEvent(SDL_Event e);
};
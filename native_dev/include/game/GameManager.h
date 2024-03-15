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

    /*
        game state variables
    */
    bool running;

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
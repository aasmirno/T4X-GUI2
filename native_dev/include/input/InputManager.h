#include <SDL2/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL2/SDL_opengles2.h>
#else
#include <SDL2/SDL_opengl.h>
#endif
#include <functional>

class InputManager
{
private:
    std::function<void(SDL_Event)> event_handler;
    bool KEYS[322]; //keyboard event state
public:
    InputManager() {}
    ~InputManager() {}

    bool* getKeys(){
        return &KEYS[0];
    }

    void set_handler(std::function<void(SDL_Event)> event_handle)
    {
        event_handler = event_handle;
    }
    bool pollEvent();
};

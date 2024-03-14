#include <SDL2/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL2/SDL_opengles2.h>
#else
#include <SDL2/SDL_opengl.h>
#endif

class InputManager{
private:
    void (*event_handler)(SDL_Event event);

public:
    InputManager(){}
    ~InputManager(){}

    void set_handler(void (*event_handle)(SDL_Event e)){
        event_handler = event_handle;
    }
    bool pollEvent();
};

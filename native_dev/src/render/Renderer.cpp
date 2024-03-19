#include "render/Renderer.h"

bool Renderer::initialise()
{
    /*
        SDL setup
    */
    {
        // Set SDL attributes to GL 4.2 + GLSL 420
        const char *glsl_version = "#version 420";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

        // Create window with graphics context
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // use double buffer
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);  // set depth buffer bits to 24
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); // set stencil buffer bits to 8

        // set window flags
        SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

        // try create sdl window
        if ((main_window = SDL_CreateWindow("Main Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_W, WINDOW_H, window_flags)) == NULL)
        {
            printf("window creation Error: %s\n", SDL_GetError());
            return false;
        }

        // try create opengl context
        if ((gl_context = SDL_GL_CreateContext(main_window)) == NULL)
        {
            printf("SDL error: %s\n", SDL_GetError());
            return false;
        }

        // focus created opengl context
        if (SDL_GL_MakeCurrent(main_window, gl_context) != 0)
        {
            printf("make_current Error: %s\n", SDL_GetError());
            return false;
        }

        // try enable vsync
        if (SDL_GL_SetSwapInterval(1) == 0)
        {
            vsync_enabled = true;
        }
        else
        {
            printf("vsync not supported\n");
        }
    }
    /*
        Glew/Gl setup
    */
    {
        // start up glew
        glewExperimental = GL_TRUE;
        GLenum glewError = glewInit();
        if (glewError != GLEW_OK)
        {
            printf("glew initialisation error: %d\n", glewError);
            return false;
        }
        printf("glew initialised\n");

        // set gl default color buffer values
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        printf("openGL ver: %s, glsl ver: %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
    }

    // check for GL errors
    GLenum err;
    if ((err = glGetError()) != GL_NO_ERROR)
    {
        printf("%d", err);
    }

    // toggle init flag
    initialised = true;
    return true;
}

void Renderer::shutdown()
{
    for (int i = 0; i < tile_objects.size(); i++)
    {
        tile_objects[i].cleanup();
    }
}

void Renderer::adj_transform(int event_value)
{
    if (!initialised)
    {
        printf("ERROR: render manager not initialised\n");
        return;
    }

    float factor = 1.0f;
    if (event_value < 0 && transform[0][0] > min_transform)
    { // scroll wheel back: zoom out
        factor = 0.9;
    }
    else if (transform[0][0] < max_transform)
    { // scroll wheel forward: zoom in
        factor = 1.1f;
    }

    transform[0][0] *= factor;
    if (transform[0][0] < min_transform)
    {
        transform[0][0] = min_transform;
    }
    if (transform[0][0] > max_transform)
    {
        transform[0][0] = max_transform;
    }

    transform[1][1] = transform[0][0];
    for (int i = 0; i < tile_objects.size(); i++)
    {
        tile_objects[i].update_transform(&transform[0][0]);
    }
}

void Renderer::move_transform(char dir)
{
    if (!initialised)
    {
        printf("ERROR: render manager not initialised\n");
        return;
    }
    float x = 0.0f; float y = 0.0f;

    float curr_move_speed = move_speed / transform[0][0];   //adjust move speed based on transform level

    switch (dir)
    {
    case 'w':
        y -= curr_move_speed;
        break;
    case 'a':
        x += curr_move_speed;
        break;
    case 's':
        y += curr_move_speed;
        break;
    case 'd':
        x -= curr_move_speed;
        break;
    default:
        break;
    }

    for (int i = 0; i < tile_objects.size(); i++)
    {
        tile_objects[i].setOffset(x, y);
    }
}

TileObject *Renderer::addTileObject(int x_dim, int y_dim, uint16_t *data, const char *texture_source, unsigned texture_w, unsigned texture_h)
{
    if (!initialised)
    {
        printf("ERROR: render manager not initialised\n");
        return nullptr;
    }

    if (data == nullptr)
    {
        printf("ERROR: bad data pointer in addTileObject");
        return nullptr;
    }

    TileObject obj;
    int id = tile_objects.size() + 1;
    if (!obj.initialise(id))
    {
        printf("Shape object initialisation failed\n");
        return nullptr;
    }                                                     // call basic initialisation function
    obj.update_transform(&transform[0][0]);               // update the transform to current transform
    obj.setData(data, x_dim, y_dim);                      // set the tile data
    obj.setTexture(texture_source, texture_w, texture_w); // load a texture

    // add it to active objects
    tile_objects.push_back(obj);
    return &tile_objects.back();
}

void Renderer::render()
{
    if (!initialised)
    {
        printf("ERROR: render manager not initialised\n");
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Draw all active objects
    for (int i = 0; i < tile_objects.size(); i++)
    {
        tile_objects[i].draw();
    }

    SDL_GL_SwapWindow(main_window);
}

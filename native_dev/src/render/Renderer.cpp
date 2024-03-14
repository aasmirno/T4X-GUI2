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

    /*
        Initialise shaders
    */
    {
        // create basic shader program
        SourcePair basic_program[] = {
            SourcePair{"resources/basic_shaders/basevert.glvs", GL_VERTEX_SHADER},
            SourcePair{"resources/basic_shaders/basefrag.glfs", GL_FRAGMENT_SHADER}};
        ShaderProgram basic = shader_manager.createProgram(&basic_program[0], 2);
        active_programs.push_back(basic);

        // create tileid map shader
        SourcePair tile_program[] = {
            SourcePair{"resources/tiled_shaders/tilevert.glvs", GL_VERTEX_SHADER},
            SourcePair{"resources/tiled_shaders/tilegeom.glgs", GL_GEOMETRY_SHADER},
            SourcePair{"resources/tiled_shaders/tilefrag.glfs", GL_FRAGMENT_SHADER}};
        ShaderProgram tiled = shader_manager.createProgram(&tile_program[0], 3);
        active_programs.push_back(tiled);
    }
    // toggle init flag
    initialised = true;
    return true;
}

void Renderer::adj_transform(int event_value){
    if (!initialised)
    {
        printf("ERROR: render manager not initialised\n");
        return;
    }

    float factor = 1.0f;
    if(event_value < 0 && transform[0][0] > min_transform){ //scroll wheel back: zoom out
        factor = 0.9;
    } else if(transform[0][0] < max_transform){ //scroll wheel forward: zoom in
        factor = 1.1f;
    }

    transform[0][0] *= factor;
    if(transform[0][0] < min_transform){
        transform[0][0] = min_transform;
    }
    if(transform[0][0] > max_transform){
        transform[0][0] = max_transform;
    }

    transform[1][1] = transform[0][0];
    for(int i = 0; i < tile_objects.size(); i++){
        tile_objects[i].update_transform(&transform[0][0]);
    }
}

bool Renderer::addRenderObject()
{
    if (!initialised)
    {
        printf("ERROR: render manager not initialised\n");
        return false;
    }

    // try to initialise a new shape object
    ShapeObject obj;
    if (obj.initialise(active_objects.size() + 1, active_programs[0].program_id) == false)
    {
        printf("failed to initialise render object\n");
        return false;
    }

    obj.addVertex(0.0f,  0.0f,  1.0f);
    obj.addVertex(0.5f, 0.0f,  1.0f);
    obj.addVertex(0.5f, -0.5f,  1.0f);

    obj.setColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    // add it to active objects
    active_objects.push_back(obj);
    printf("created render_object=%d\n", active_objects.size());

    return true;
}

bool Renderer::addTileObject(int x_dim, int y_dim)
{
    if (!initialised)
    {
        printf("ERROR: render manager not initialised\n");
        return false;
    }

    TileObject obj;
    obj.initialise(tile_objects.size() + 1, active_programs[1].program_id);
    obj.update_transform(&transform[0][0]);
    obj.setDims(x_dim, y_dim);

    // add it to active objects
    tile_objects.push_back(obj);
    printf("created render_object=%d\n", active_objects.size());
    return true;
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
    for (int i = 0; i < active_objects.size(); i++)
    {
       active_objects[i].draw();
    }
    for (int i = 0; i < tile_objects.size(); i++)
    {
        tile_objects[i].draw();
    }

    SDL_GL_SwapWindow(main_window);
}

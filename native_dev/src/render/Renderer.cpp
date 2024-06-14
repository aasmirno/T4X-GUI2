#include "render/Renderer.h"

bool Renderer::initialise(int screen_height, int screen_width)
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
            printf("    vsync not supported\n");
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
            printf("    glew initialisation error: %d\n", glewError);
            return false;
        }
        printf("    glew initialised\n");

        // set gl default color buffer values
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);

        glEnable(GL_CULL_FACE); // Cull back facets
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        WINDOW_H = screen_height;
        WINDOW_W = screen_width;
        printf("    Supported OpenGL version: %s, glsl ver: %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
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
    for (int i = 0; i < texture_objects.size(); i++)
    {
        texture_objects[i].cleanup();
    }
}

void Renderer::updateTransform(){
    for (int i = 0; i < texture_objects.size(); i++)
    {
        texture_objects[i].setTransform(&transform[0][0]);
    }
    for (int i = 0; i < m_objects.size(); i++)
    {
        m_objects[i].setTransform(&transform[0][0]);
    }
}

void Renderer::setScale(int event_value)
{
    if (!initialised)
    {
        printf("ERROR: render manager not initialised\n");
        return;
    }

    float factor = 1.0f;
    if (event_value < 0 && transform[0][0] > min_scale * adjustment_factor_H)
    { // scroll wheel back: zoom out
        factor = 0.9;
    }
    else if (transform[0][0] < max_scale * adjustment_factor_H)
    { // scroll wheel forward: zoom in
        factor = 1.1f;
    }

    transform[0][0] *= factor;
    if (transform[0][0] < min_scale * adjustment_factor_H)
    {
        transform[0][0] = min_scale * adjustment_factor_H;
    }
    if (transform[0][0] > max_scale * adjustment_factor_H)
    {
        transform[0][0] = max_scale * adjustment_factor_H;
    }

    transform[1][1] *= factor;
    if (transform[1][1] < min_scale * adjustment_factor_W)
    {
        transform[1][1] = min_scale * adjustment_factor_W;
    }
    if (transform[1][1] > max_scale * adjustment_factor_W)
    {
        transform[1][1] = max_scale * adjustment_factor_W;
    }

    updateTransform();
}

void Renderer::setTranslation(char direction)
{
    if (!initialised)
    {
        printf("ERROR: render manager not initialised\n");
        return;
    }
    float x = 0.0f;
    float y = 0.0f;

    // adjust move speed based on scaling level
    float curr_move_speed = move_speed / transform[0][0];

    switch (direction)
    {
    case 'w':
        transform[3][1] -= curr_move_speed;
        break;
    case 'a':
        transform[3][0] += curr_move_speed;
        break;
    case 's':
        transform[3][1] += curr_move_speed;
        break;
    case 'd':
        transform[3][0] -= curr_move_speed;
        break;
    case 'i':
        transform[3][2] += curr_move_speed;
        break;
    case 'j':
        transform[3][2] -= curr_move_speed;
    default:
        break;
    }

    std::cout << glm::to_string(transform) << "\n";
    updateTransform();
}

void Renderer::setRotation(rAxis axis, rDirection direction){
    if(axis == RD_Z && direction == RD_LEFT){
        //transform[0][0] *= cos_t;
        //transform[1][0] *= sin_t;
        //transform[0][1] *= -sin_t;
        //transform[1][1] *= cos_t;
    }

    std::cout << glm::to_string(transform) << "\n";

    updateTransform();
}


void Renderer::setScreenSize(int width, int height)
{
    transform[0][0] = (transform[0][0] * WINDOW_H / height);
    transform[1][1] = (transform[1][1] * WINDOW_W / width);
    adjustment_factor_H = (adjustment_factor_H * WINDOW_H / height);
    adjustment_factor_W = (adjustment_factor_W * WINDOW_W / width);
    WINDOW_W = width;
    WINDOW_H = height;

    glViewport(0, 0, WINDOW_H, WINDOW_W);
}

TexturedObject *Renderer::addTexturedObject(const char *texture_source, unsigned texture_w, unsigned texture_h)
{
    if (!initialised)
    {
        printf("ERROR: render manager not initialised\n");
        return nullptr;
    }

    TexturedObject object;
    int id = texture_objects.size() + 1;
    if (!object.initialise(id))
    {
        printf("Shape object initialisation failed\n");
        return nullptr;
    }

    object.setTransform(&transform[0][0]);               // update the transform to current transform
    object.setTexture(texture_source, texture_w, texture_w); // load a texture

    // add it to active objects
    texture_objects.push_back(object);
    return &texture_objects.back();
}

void Renderer::render()
{
    if (!initialised)
    {
        printf("ERROR: render manager not initialised\n");
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT);
    
    // Draw all active objects
    for (int i = 0; i < m_objects.size(); i++)
    {
        m_objects[i].draw();
    }
    for (int i = 0; i < texture_objects.size(); i++)
    {
        texture_objects[i].draw();
    }

    SDL_GL_SwapWindow(main_window);
}

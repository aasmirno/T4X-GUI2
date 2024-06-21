#include "T4X/render/Renderer.h"

void Renderer::keyUpdate(RENDER_KEY_STATE state){
    if(state.W){
        camera.rotate(0.01, glm::vec3(1.0, 0.0, 0.0));
    }

    if(state.A){
        camera.rotate(-0.01, glm::vec3(0.0, 1.0, 0.0));
    }

    if(state.S){
        camera.rotate(-0.01, glm::vec3(1.0, 0.0, 0.0));
    }

    if(state.D){
        camera.rotate(0.01, glm::vec3(0.0, 1.0, 0.0));
    }
    updateView();
}

void Renderer::eventUpdate(Event e)
{
    if (e.type != E_TYPE::RENDER_EVENT)
    {
        printf("[ RENDER ERROR ] event supplied of improper type");
        return;
    }
    
    std::cout << "test\n";
    switch (e.render_data)
    {
    case MW_IN:
        camera.translate(glm::vec3(0.0f, 0.0f, 1.0f));
        break;
    case MW_OUT:
        camera.translate(glm::vec3(0.0f, 0.0f, -1.0f));
        break;
    default:
        break;
    }

    updateView();
}

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

        /*
            GL parameters
        */
        // set gl default color buffer values
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);

        // culling parameters
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);

        // blending parameters
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // z buffer parameters
        glEnable(GL_DEPTH_TEST);

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

    // initialise camera
    camera.initialise();

    projection = glm::perspective(glm::radians(120.0f), (float)WINDOW_W / WINDOW_H, 0.1f, 100.0f);

    //  toggle init flag
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

void Renderer::updateView()
{
    GLfloat *curr_view = camera.getView();
    for (int i = 0; i < texture_objects.size(); i++)
    {
        texture_objects[i].setTransform(curr_view, "view");
    }
    for (int i = 0; i < m_objects.size(); i++)
    {
        m_objects[i].setTransform(curr_view, "view");
    }
}

void Renderer::updateProjection()
{
    GLfloat *curr_view = camera.getView();
    for (int i = 0; i < texture_objects.size(); i++)
    {
        texture_objects[i].setTransform(&projection[0][0], "projection");
    }
    for (int i = 0; i < m_objects.size(); i++)
    {
        m_objects[i].setTransform(&projection[0][0], "projection");
    }
}

void Renderer::setScreenSize(int width, int height)
{
    projection[0][0] = (projection[0][0] * WINDOW_H / height);
    projection[1][1] = (projection[1][1] * WINDOW_W / width);
    adjustment_factor_H = (adjustment_factor_H * WINDOW_H / height);
    adjustment_factor_W = (adjustment_factor_W * WINDOW_W / width);
    WINDOW_W = width;
    WINDOW_H = height;
}

TexturedObject *Renderer::addTexturedObject(const char *texture_source, unsigned texture_w, unsigned texture_h)
{
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
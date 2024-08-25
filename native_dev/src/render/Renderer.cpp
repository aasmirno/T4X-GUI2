#include "T4X/render/Renderer.h"

void Renderer::keyUpdate(RENDER_KEY_STATE state){
    //wireframe
    if (state.P && !wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // move controls
    if (state.A) camera.left();
    if (state.D) camera.right();
    if (state.W) camera.top();
    if (state.S) camera.bottom();

    // yaw controls
    if (state.Q) camera.moveYaw(0.5f);
    if (state.E) camera.moveYaw(-0.5f);

    // pitch controls
    if (state.R) camera.movePitch(0.5f);
    if (state.F) camera.movePitch(-0.5f);

    updateView();
}

void Renderer::eventUpdate(Event e)
{
    if (e.type != E_TYPE::RENDER_EVENT)
    {
        printf("[ RENDER ERROR ] event supplied of improper type");
        return;
    }
    switch (e.render_data)
    {
    case MW_IN:
        camera.in();
        updateView();
        break;
    case MW_OUT:
        camera.out();
        updateView();
        break;
        
    default:
        break;
    }
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
        //glCullFace(GL_FRONT_AND_BACK);

        // blending parameters
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // z buffer parameters
        glEnable(GL_DEPTH_TEST);

        WINDOW_H = screen_height;
        WINDOW_W = screen_width;
        printf("    Supported OpenGL version: %s, glsl ver: %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
    }

    /*
        imgui setup
    */
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        
        // Setup Platform/Renderer backends
        ImGui_ImplSDL2_InitForOpenGL(main_window, gl_context);
        ImGui_ImplOpenGL3_Init();
    }


    printf("    Finishing Initialisation\n");
    // check for GL errors
    GLenum err;
    if ((err = glGetError()) != GL_NO_ERROR)
    {
        printf("%d", err);
    }

    // initialise camera
    camera.initialise();

    projection = glm::perspective(glm::radians(90.0f), (float)WINDOW_W / WINDOW_H, 0.1f, 100.0f);
    //projection = glm::ortho(0.0f, (float)WINDOW_W, 0.0f, (float)WINDOW_H, 0.1f, 10.0f);
    //  toggle init flag
    initialised = true;

    std::shared_ptr<Menu> menu = std::make_unique<MainMenu>();
    menu_stack.push(menu);

    printf("Done\n");
    return true;
}

void Renderer::shutdown()
{
    // call cleanup methods
    for (auto& iterator : world_objects) {
        iterator.second->cleanup();
    }
    for (auto& iterator : world_objects) {
        iterator.second->cleanup();
    }

    // clear vectors and maps
    texture_objects.clear();
    meshes.clear();
    t_obj.clear();

    world_objects.clear();
    flat_objects.clear();


    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void Renderer::setScreenSize(int width, int height)
{
    WINDOW_W = width;
    WINDOW_H = height;
    glViewport(0, 0, WINDOW_W, WINDOW_H);
}

RenderObject* Renderer::addTexturedObject(uint id)
{
    if (world_objects.find(id) != world_objects.end()) {
        printf("[RENDERER ERROR] object with id %d already exists\n", id);
        return nullptr;
    }

    // initialise a textured object
    TexturedObject obj;
    if (!obj.initialise(id)) return nullptr;

    // add to ds and return
    texture_objects.push_back(obj);
    flat_objects[id] = &texture_objects.back();
    return &texture_objects.back();
}

bool Renderer::setTexture(uint id, const char* filename) {
    for (int i = 0; i < texture_objects.size(); i++) {
        if (texture_objects[i].object_id == id) texture_objects[i].loadTexture(filename);
    }
    return true;
}

WorldObject* Renderer::addMeshObject(uint id)
{
    if (world_objects.find(id) != world_objects.end()) {
        printf("[R_MNGR ERROR] object with id %d already exists\n", id);
        return nullptr;
    }
    //initialise a mesh object
    MeshObject obj;
    if (!obj.initialise(id)) return nullptr;
    obj.setAmbient(1.0f);
    obj.setLightPosition(20.0f, 20.0f, 20.0f);

    // add to ds and return
    meshes.push_back(obj);
    world_objects[id] = &meshes.back();
    updateView(); updateProjection();
    return &meshes.back();
}

bool Renderer::setMeshData(uint id, float* data, int width, int height) {
    if (world_objects.find(id) == world_objects.end()) {
        printf("[R_MNGR ERROR] mesh object with id %d not found\n", id);
        return false;
    }
    
    //try to cast object at [id] to a mesh object
    auto object = dynamic_cast<MeshObject*>(world_objects[id]);
    if (!object) {
        printf("[R_MNGR ERROR] object with id %d is not a mesh object\n", id);
        return false;
    }

    return object->setMeshData(data, width, height);
}


WorldObject* Renderer::addTestObject()
{
    TestObject obj;
    if (!obj.initialise(0)) return nullptr;

    t_obj.push_back(obj);
    world_objects[0] = &t_obj.back();
    updateView(); updateProjection();

    return &t_obj.back();
}

void Renderer::updateView()
{
    GLfloat* curr_view = camera.getView();
    for (auto& iterator : world_objects) {
        iterator.second->setTransform(curr_view, "view");
    }
}

void Renderer::updateProjection()
{
    for (auto& iterator : world_objects) {
        iterator.second->setTransform(&projection[0][0], "projection");
    }
}

void Renderer::render()
{

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // draw menus
    menu_stack.top()->draw();

    if (!initialised)
    {
        printf("ERROR: render manager not initialised\n");
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw all active objects
    for (auto& iterator : world_objects) {
        iterator.second->draw();
    }

    for (auto& iterator : flat_objects) {
        iterator.second->draw();
    }

    // draw imgui objects
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(main_window);
}
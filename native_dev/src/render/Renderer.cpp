#include "Renderer.h"

bool Renderer::initialise()
{
    /*
        SDL setup
    */
    {
        // initialise all sdl modules
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        {
            printf("sdl init error: %s\n", SDL_GetError());
            return false;
        }

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
        } else {
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

    //check for GL errors
	GLenum err;
	if ((err = glGetError()) != GL_NO_ERROR) {
		printf("%d", err);
	}
	
    //create basic shader program
    createProgram();

    //toggle init flag
    initialised = true;
	return true;
}

bool Renderer::addRenderObject(){
    if(!initialised){
        printf("render manager not initialised\n");
        return false;
    }

    // try to initialise a new shape object
    ShapeObject obj;
    if(obj.initialise(active_objects.size() + 1, 3) == false){
        printf("failed to initialise render object\n");
        return false;
    }
    obj.addVertex(active_objects.size(),  0.5f,  0.0f);
    obj.addVertex(0.5f, -0.5f,  0.0f);
    obj.addVertex(-0.5f, -0.5f,  0.0f);

    obj.setColor(Vec4((active_objects.size() + 1)*100 / 256.0, 0.0f, 0.0f, 1.0f));

    //add it to active objects
    active_objects.push_back(obj);
    printf("created render_object=%d\n", active_objects.size());
    obj.printDebug();
    return true;
}

void Renderer::render(){
    if(!initialised){
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Draw all active objects
    for(int i = 0; i < active_objects.size(); i++){
        active_objects[i].draw();
    }

    SDL_GL_SwapWindow(main_window);
}

GLuint Renderer::loadShader(std::string path, GLenum type){
	GLuint shader_id = 0;

	std::string shader_raw;
	std::ifstream file_source(path.c_str());

	if (file_source) {
		//read file
		shader_raw.assign((std::istreambuf_iterator< char >(file_source)), std::istreambuf_iterator< char >());

		//create shader and set source
		shader_id = glCreateShader(type);

		const GLchar* shader_source = shader_raw.c_str();
		glShaderSource(shader_id, 1, (const GLchar**)&shader_source, NULL);

		//compile
		glCompileShader(shader_id);

		//error checking
		GLint compiled = GL_FALSE;
		glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled);
		if (compiled != GL_TRUE) {
			printf("    shader %d compile error:\n%s\n", shader_id, shader_source);

			//cleanup on comp failure
			glDeleteShader(shader_id);
			return 0;
		}
	}
	else {
		printf("    shader creation error: \n   bad file: %s\n", path.c_str());
		return 0;
	}

	//check shader created
	if (shader_id == 0) {
		printf("    shader creation error\n");
		return 0;
	}

    printf("    success loading shader=%d from %s\n", shader_id, path.c_str());
	return shader_id;
}

bool Renderer::createProgram(){
    printf("creating shader program:\n");
    //load vertex and fragment shader hardcoded
    GLuint vs = loadShader("resources/basevert.glvs", GL_VERTEX_SHADER);
    if(vs == 0) {return false;}
    GLuint fs = loadShader("resources/basefrag.glfs", GL_FRAGMENT_SHADER);
    if(fs == 0) {glDeleteShader(vs); return false;}

    GLuint program = glCreateProgram(); //create new gl context program
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    ShaderProgram new_program{program, vs, fs};
    active_programs.push_back(new_program);

    printf("created shader program: %d\n", program);
    return true;
}

#include "MainWindow.h"

MainWindow::MainWindow() {
	clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	Running = true;
	main_window = NULL;
	gl_context = NULL;
	im_context = NULL;

}

bool MainWindow::Init() {
	// Setup SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return false;
	}

	// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
	const char* glsl_version = "#version 100";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
	// GL 3.2 Core + GLSL 150
	const char* glsl_version = "#version 150";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

	// From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

	/*
		OpenGL + SDL setup
	*/
	// Create window with graphics context
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);	//use double buffer
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);		//set depth buffer bits to 24
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);	//set stencil buffer bits to 8

	//set window flags
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

	//try create sdl window
	if ((main_window = SDL_CreateWindow("Main Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags)) == NULL) {
		printf("Error: %s\n", SDL_GetError());
		return false;
	}

	//try create opengl context
	if ((gl_context = SDL_GL_CreateContext(main_window)) == NULL) {
		printf("Error: %s\n", SDL_GetError());
		return false;
	}

	//try set openlgl context
	if (SDL_GL_MakeCurrent(main_window, gl_context) != 0) {
		printf("Error: %s\n", SDL_GetError());
		return false;
	}

	//try enable vsync
	if (SDL_GL_SetSwapInterval(1) != 0) {
		printf("Error: %s\n", SDL_GetError());
		return false;
	}

	/*
		Glew setup
	*/
	//start up glew
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		printf("glew error: ");
	}

	//init glew loader
	if (glew.init() != true) {
		printf("glew loader error\n");
		return false;
	}

	/*
		ImGui setup
	*/
	IMGUI_CHECKVERSION();
	im_context = ImGui::CreateContext();
	// Setup Dear ImGui style
	ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(main_window, gl_context);
	ImGui_ImplOpenGL3_Init(glsl_version);

	return true;
}

int MainWindow::Start() {
	if (Init() != true) {
		return -1;
	}

	//set gl deafult color buffer values
	clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);


	// Setup Dear ImGui context
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls


	while (Running) {

		//get events with sdl
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				Running = false;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(main_window))
				Running = false;
		}

		//render some image
		Render();

		//conduct game loop
		Loop();

	}

	return 0;
}

void MainWindow::Loop() {

}

void MainWindow::Render() {
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);

	/*
		Glew test
	*/
	glUseProgram(glew.getID());
	glEnableVertexAttribArray(glew.getVpos());

	glBindBuffer(GL_ARRAY_BUFFER, glew.getVBO());
	glVertexAttribPointer(glew.getVpos(), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glew.getIBO());
	glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL);

	glDisableVertexAttribArray(glew.getVpos());

	glUseProgram(NULL);

	//-----------------------------------------------------


	/*
		Imgui rendering
	*/
	//create necessary frames
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();

	//render imgui components
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	//-----------------------------------------------------

	//update window
	SDL_GL_SwapWindow(main_window);
}

void MainWindow::Cleanup() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(main_window);
	SDL_Quit();
}
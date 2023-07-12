#include "MainController.h"

MainController::MainController() {
}

bool MainController::genInit() {
	// Setup SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		printf("sdl init error: %s\n", SDL_GetError());
		return false;
	}

	// GL 4.5 + GLSL 450
	const char* glsl_version = "#version 450";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

	// From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

	/*
		SDL setup
	*/
	// Create window with graphics context
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);	//use double buffer
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);		//set depth buffer bits to 24
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);	//set stencil buffer bits to 8

	//set window flags
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

	//try create sdl window
	if ((main_window = SDL_CreateWindow("Main Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_W, WINDOW_H, window_flags)) == NULL) {
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

	printf("sdl initialised\n");
	/*
		Glew/Gl setup
	*/
	//start up glew
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		printf("glew error: %d\n", glewError);
		return false;
	}
	printf("glew initialised\n");

	//set gl default color buffer values
	clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);

	printf("openGL ver: %s, glsl ver: %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

	/*
		ImGui setup
	*/
	IMGUI_CHECKVERSION();
	im_context = ImGui::CreateContext();

	//Platform/Renderer backend setup
	ImGui_ImplSDL2_InitForOpenGL(main_window, gl_context);
	ImGui_ImplOpenGL3_Init(glsl_version);

	/*
		Internal setup
	*/
	//glViewport(cam.getX(), cam.getY(), WINDOW_W, WINDOW_H);

	//check for errors
	if (glGetError() != GL_NO_ERROR) {
		printf("opengl error\n");
		return false;
	}

	return true;
}

int MainController::Start() {
	//try to initialise required systems
	if (genInit() != true) {
		return -1;
	}

	// Setup Dear ImGui io
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Control

	//start run loop
	while (Running) {
		SDL_GetWindowSize(main_window, &WINDOW_W, &WINDOW_H);
		//get events with sdl
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			//SDL window control exits
			if (event.type == SDL_QUIT)
				Running = false;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(main_window))
				Running = false;

			//send events to imgui if required
			if (io.WantCaptureKeyboard || io.WantCaptureMouse) {
				ImGui_ImplSDL2_ProcessEvent(&event);
			}
			else {
				//event handler
				EventHandle(event);
			}
		}
		if (ft) {
			cam.zoomIn();
			cam.zoomOut();
			ft = false;
		}

		//render graphics
		Render();

		//conduct game loop
		GameLoop();
	}

	return 0;
}

void MainController::EventHandle(SDL_Event& event) {
	bool wantMouse = ImGui::GetIO().WantCaptureMouse;
	bool wantKey = ImGui::GetIO().WantCaptureKeyboard;

	//window resize logic
	if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
		//glViewport(cam.getX(), cam.getY(), WINDOW_W, WINDOW_H);
		//cam.move(SDLK_m, (GLint)WINDOW_W, (GLint)WINDOW_H);
	}


	//camera zoom logic
	if (draw_game) {
		if (event.type == SDL_MOUSEWHEEL && !wantMouse && draw_start != true) {
			if (event.wheel.y < 0) { //scroll back
				cam.zoomOut();
			}
			else if (event.wheel.y > 0) { //scroll forward
				cam.zoomIn();
			}
		}

		//camera wasd movement logic
		if ((event.key.keysym.sym == SDLK_w ||
			event.key.keysym.sym == SDLK_s ||
			event.key.keysym.sym == SDLK_a ||
			event.key.keysym.sym == SDLK_d) && !wantKey)
		{
			cam.move(event.key.keysym.sym);
		}
	}

	Sleep(1);
}

void MainController::Render() {
	//clear gl buffers
	glClear(GL_COLOR_BUFFER_BIT);
	/*
		Imgui rendering
	*/
	//create necessary frames
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	//drawing logic
	if (draw_game) {
		if (ft) {
			cam.zoomIn();
			cam.zoomOut();
		}

		game.Draw();
	}
	else {
		menuHandler();
	}

 	if (debug)
		DebugMenu();

	//render imgui components
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	//-----------------------------------------------------

	//update window
	SDL_GL_SwapWindow(main_window);
}

void MainController::GameLoop() {

}

void MainController::Cleanup() {
	//shutdown imgui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	glUseProgram(NULL);	//unassign gl programs

	//shutdown sdl
	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(main_window);
	SDL_Quit();
}

/*
	ImGui window drawing methods
*/

//handle menu drawing logic
void MainController::menuHandler() {
	if (draw_start)
		drawStart();
	if (draw_options)
		drawOpts();

	if (draw_demo)
		ImGui::ShowDemoWindow();
}

void MainController::DebugMenu() {
	ImGui::Begin("Debug");
	ImGui::SeparatorText("Window and camera");
	ImGui::Text("window size w: %d h: %d", WINDOW_W, WINDOW_H);
	ImGui::Text("cam pos x,y: (%f,%f)", cam.getX(), cam.getY());
	ImGui::Text("current zoom (transform multiplier): %f ", cam.getZoom());
	if (ImGui::Button("reset cam")) {
		cam.reset();
	}

	ImGui::SeparatorText("imgui controls");
	ImGui::Text("imgui io flags wantMouse: %d wantKeyboard: %d", ImGui::GetIO().WantCaptureMouse, ImGui::GetIO().WantCaptureKeyboard);

	ImGui::SeparatorText("Game state values");
	ImGui::Text("loadRequest: %d", false);
	ImGui::End();
}

void MainController::drawStart() {
	assert(draw_options == false);

	ImGuiWindowFlags flags = 0;
	flags |= ImGuiWindowFlags_NoMove;
	flags |= ImGuiWindowFlags_NoResize;

	ImGui::SetNextWindowPos(ImVec2{ 10,10 });

	ImGui::Begin("Main Menu", NULL, flags);

	//new button
	if (ImGui::Button("New Game")) {
		draw_game = true;
		draw_start = false;
		game.init();
		cam.setTransformLoc(game.getMapTransformLoc());
		ft = true;
	}

	//loading button
	if (ImGui::Button("Load")) {

	}

	//options
	if (ImGui::Button("Options")) {
		draw_options = true;
		draw_start = false;
	}

	//exit
	if (ImGui::Button("Exit")) {
		Running = false;
	}

	ImGui::End();
}

void MainController::drawOpts() {
	ImGuiWindowFlags flags = 0;
	flags |= ImGuiWindowFlags_NoMove;
	flags |= ImGuiWindowFlags_NoResize;

	ImGui::SetNextWindowPos(ImVec2{ 10,10 });

	ImGui::Begin("Options", NULL, flags);

	//go back to main menu
	if (ImGui::Button("Back")) {
		draw_options = false;
		draw_start = true;
	}

	//show imgui demo window
	ImGui::Checkbox("Show ImGui demo window", &draw_demo);
	ImGui::End();
}
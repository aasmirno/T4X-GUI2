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

	//set style
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowPadding = ImVec2(0, 0);
	style.ItemSpacing = ImVec2(4, 1);


	/*
		Internal setup
	*/
	GLenum err;
	//check for errors
	if ((err = glGetError()) != GL_NO_ERROR) {
		printf("%d", err);
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
		SDL_GetMouseState(&mouse_x, &mouse_y);
		game.setMouse(cam.windowToTileX(mouse_x, WINDOW_W), cam.windowToTileY(mouse_y, WINDOW_H));
		if (exit_flag) {
			draw_start = true;
			draw_game = false;
			exit_flag = false;
		}

		//get events with sdl
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			//SDL window control exits
			if (event.type == SDL_QUIT)
				Running = false;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(main_window))
				Running = false;

			//options menu
			if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_ESCAPE] && draw_game) {
				if (!draw_pause) {
					draw_pause = true;
				}
				else {
					game.unPause();
					draw_pause = false;
				}
			}

			if (!game.getFrozen()) {
				//send events to imgui if required
				if (io.WantCaptureKeyboard || io.WantCaptureMouse) {
					mouse1_pressed = false;
					ImGui_ImplSDL2_ProcessEvent(&event);
				}
				else {
					//event handler
					EventHandle(event);
				}
			}
			else if (mouse_x >= WINDOW_W / 2 - 200 && mouse_y >= WINDOW_H / 2 - 200 && mouse_x < WINDOW_W / 2 + 200 && mouse_y < WINDOW_H / 2 + 200) {
				ImGui_ImplSDL2_ProcessEvent(&event);
			}
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
	const Uint8* keyboard_state_array = SDL_GetKeyboardState(NULL);

	//window resize logic
	if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
		//glViewport(cam.getX(), cam.getY(), WINDOW_W, WINDOW_H);
		//cam.move(SDLK_m, (GLint)WINDOW_W, (GLint)WINDOW_H);
	}


	//camera movement logic
	if (draw_game) {
		//zoom logic
		if (event.type == SDL_MOUSEWHEEL && !wantMouse && draw_start != true) {
			if (event.wheel.y < 0) { //scroll back
				cam.zoomOut();
			}
			else if (event.wheel.y > 0) { //scroll forward
				cam.zoomIn(cam.windowToCam(mouse_x, WINDOW_W), cam.windowToCam(mouse_y, WINDOW_H));
			}
		}

		//camera mouse movement logic
		if (event.type == SDL_MOUSEBUTTONDOWN) {
			if (event.button.button == SDL_BUTTON_LEFT) {	//left click (m1)
				mouse1_pressed = true;
			}
			else if (event.button.button == SDL_BUTTON_RIGHT) {	//right click (m2)

			}
		}
		if (event.type == SDL_MOUSEBUTTONUP) {
			if (event.button.button == SDL_BUTTON_LEFT) {	//left click (m1)
				mouse1_pressed = false;
			}
			else if (event.button.button == SDL_BUTTON_RIGHT) {	//right click (m2)

			}
		}
		if (mouse1_pressed && !wantMouse && event.type == SDL_MOUSEMOTION) {
			cam.move(event);
		}

		//camera wasd movement logic
		if ((event.key.keysym.sym == SDLK_w ||
			event.key.keysym.sym == SDLK_s ||
			event.key.keysym.sym == SDLK_a ||
			event.key.keysym.sym == SDLK_d) && !wantKey)
		{
			cam.move(event);
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
		game.Draw();
		if (draw_pause)
			drawPauseMenu();
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
	game.loop();
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

	ImGui::SeparatorText("Window");
	ImGui::Text("window size w: %d h: %d", WINDOW_W, WINDOW_H);

	ImGui::SeparatorText("camera values");

	if (draw_game) {
		if (ImGui::Button("reset cam")) {
			cam.reset(340, WINDOW_W, 106, WINDOW_H);
		}
	}
	ImGui::Text("mouse pos window(%d,%d), cam(%f,%f), map(%d, %d), index(%d)", mouse_x, mouse_y, cam.windowToCam(mouse_x, WINDOW_W), cam.windowToCam(mouse_y, WINDOW_H), cam.windowToTileX(mouse_x, WINDOW_W), cam.windowToTileY(mouse_y, WINDOW_H), cam.windowToTileY(mouse_y, WINDOW_H) * 600 + cam.windowToTileX(mouse_x, WINDOW_W));
	ImGui::Text("cam pos (org) xm,ym: (%f,%f), xw,yw (%f,%f)", cam.getX(), cam.getY(), cam.getWindowX(WINDOW_W), cam.getWindowY(WINDOW_H));
	float dx = cam.windowToCam(mouse_x, WINDOW_W) - cam.getX();
	float dy = cam.windowToCam(mouse_y, WINDOW_H) - cam.getY();
	float r1x = dx / 6000;
	float r1y = dy / 6000;

	ImGui::Text("dif dx,dy(%f,%f), /x,/y(%f,%f), /z(%f,%f)", dx, dy, r1x, r1y, r1x / cam.getZoom(), r1y / cam.getZoom());
	ImGui::Text("dist mpos to: origin(%f,%f), 0,0 (%d,%d)", mouse_x - cam.getX(), mouse_y - cam.getY(), mouse_x - (int)(WINDOW_W / 2), mouse_y - (int)(WINDOW_H / 2));
	ImGui::Text("current zoom (transform multiplier): %f ", cam.getZoom());
	ImGui::Text("mouse1_pressed: %d", mouse1_pressed);

	ImGui::SeparatorText("imgui controls");
	ImGui::Text("imgui io flags wantMouse: %d wantKeyboard: %d", ImGui::GetIO().WantCaptureMouse, ImGui::GetIO().WantCaptureKeyboard);

	ImGui::End();
}

void MainController::drawStart() {
	assert(draw_options == false && draw_game == false);

	ImGuiWindowFlags flags = 0;
	flags |= ImGuiWindowFlags_NoMove;
	flags |= ImGuiWindowFlags_NoResize;
	flags |= ImGuiWindowFlags_NoCollapse;

	ImGui::SetNextWindowPos(ImVec2{ (float)WINDOW_W / 2 - 200,(float)WINDOW_H / 2 - 200 });
	ImGui::SetNextWindowSize(ImVec2{ 400,400 });

	ImGui::Begin("Main Menu", NULL, flags);

	//new button
	if (ImGui::Button("New Game", ImVec2{ 400,30 })) {
		draw_game = true;
		draw_start = false;
		game.init(exit_flag);
		cam.setTransformLoc(game.getMapTransformLoc());
		cam.reset(340, WINDOW_W, 106, WINDOW_H);
	}

	//loading button
	if (ImGui::Button("Load Game", ImVec2{ 400,30 })) {

	}

	//options
	if (ImGui::Button("Options", ImVec2{ 400,30 })) {
		draw_options = true;
		draw_start = false;
	}

	//exit
	if (ImGui::Button("Exit", ImVec2{ 400,30 })) {
		Running = false;
	}

	ImGui::End();
}

void MainController::drawOpts() {
	ImGuiWindowFlags flags = 0;
	flags |= ImGuiWindowFlags_NoMove;
	flags |= ImGuiWindowFlags_NoResize;
	flags |= ImGuiWindowFlags_NoCollapse;

	ImGui::SetNextWindowPos(ImVec2{ (float)WINDOW_W / 2 - 200,(float)WINDOW_H / 2 - 200 });
	ImGui::SetNextWindowSize(ImVec2{ 400,400 });

	ImGui::Begin("Options", NULL, flags);

	//show internal debug
	ImGui::Checkbox("Show internal debug menu", &debug);

	//show imgui demo window
	ImGui::Checkbox("Show ImGui demo window", &draw_demo);
	//go back to main menu
	if (ImGui::Button("Back", ImVec2{ 400,30 })) {
		draw_options = false;
		draw_start = true;
	}
	ImGui::End();
}

void MainController::drawPauseMenu() {
	game.Pause();
	ImGuiWindowFlags flags = 0;
	flags |= ImGuiWindowFlags_NoMove;
	flags |= ImGuiWindowFlags_NoResize;
	flags |= ImGuiWindowFlags_NoCollapse;

	ImGui::SetNextWindowPos(ImVec2{ (float)WINDOW_W / 2 - 200,(float)WINDOW_H / 2 - 200 });

	ImGui::SetNextWindowSize(ImVec2{ 400,400 });
	ImGui::Begin("Options", NULL, flags);

	//show internal debug
	ImGui::Checkbox("Show internal debug menu", &debug);

	//show imgui demo window
	ImGui::Checkbox("Show ImGui demo window", &draw_demo);
	//go back to main menu
	if (ImGui::Button("Exit to main Menu", ImVec2{ 400,30 })) {
		draw_game = false;
		draw_start = true;
		draw_pause = false;
		game.unPause();
	}
	ImGui::End();
}
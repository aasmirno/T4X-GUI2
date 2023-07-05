#pragma once
#include "Menus.h"
#include "ShaderManager.h"
#include "Camera.h"

#include <stdio.h>
#include <iostream>
#include <vector>

#include <gl/glew.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

/*
*	Implementation of main window and game instantiation
*/
class MainWindow {
private:
	const int WINDOW_W = 1280;
	const int WINDOW_H = 720;
	SDL_Window* main_window = NULL;	//sdl window pointer
	SDL_GLContext gl_context = NULL;	//sdl-opengl context
	ImGuiContext* im_context = NULL;	//imgui context

	ShaderManager shaders;	//glew shader loader
	Menus menu_handler;

	Camera cam;	//camera controller

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);	//background color
	bool Running = true;	//run flag
	bool debug = true;

	float projection_scale = 1.0f;


private:
	bool Init();

	void Render();

	void Loop();

	void Cleanup();

	void EventHandle(SDL_Event& event);

	void DebugMenu();

public:
	MainWindow();

	int Start();

};

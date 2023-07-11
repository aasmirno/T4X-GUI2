#pragma once
#include "Camera.h"
#include "GameControl.h"

#include <stdio.h>
#include <iostream>
#include <vector>
#include <windows.h> 

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
class MainController {
private:
	//sdl values
	int WINDOW_W = 1900;
	int WINDOW_H = 1030;

	SDL_Window* main_window = NULL;		//sdl window pointer
	SDL_GLContext gl_context = NULL;	//sdl-opengl context
	ImGuiContext* im_context = NULL;	//imgui context

	//unit managers
	Camera cam;				//camera controller
	GameControl game;		//game controller
	bool ft;

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);	//background color
	bool Running = true;	//run flag
	bool debug = true;		//debug flag

	//Imgui drawing flags
	bool draw_demo = true;
	bool draw_start = true;
	bool draw_options = false;
	bool draw_game = false;

private:
	/*
		Initialise graphical systems and contexts, game control
	*/
	bool genInit();

	/*
		Render graphical elements
	*/
	void Render();

	/*
		call game loop
	*/
	void GameLoop();

	/*
		clear and free grpahical systems
	*/
	void Cleanup();

	/*
		Handle sdl provided events
	*/
	void EventHandle(SDL_Event& event);


	//imgui window draw methods ===============

	/*
		Control imgui windows
	*/
	void menuHandler();

	/*
		Imgui menus
	*/
	void DebugMenu();
	void drawStart();
	void drawOpts();

public:
	MainController();
	int Start();
};

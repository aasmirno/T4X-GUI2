#pragma once
#include "Camera.h"
#include "GameControl.h"

#include <stdio.h>
#include <iostream>
#include <vector>
#include <windows.h> 

#include "ShaderManager.h";
#include "Logger.h"
#include "EventManager.h"
#include "MenuHandler.h"

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
	/*
		sdl values
	*/
	int WINDOW_W = 1900;
	int WINDOW_H = 1030;

	SDL_Window* main_window = NULL;		//sdl window pointer
	SDL_GLContext gl_context = NULL;	//sdl-opengl context
	ImGuiContext* im_context = NULL;	//imgui context

	/*
		Unit Manager Section
	*/
	//camera controller
	Camera cam;
	int mouse_x = 0;
	int mouse_y = 0;
	bool mouse1_pressed = false;

	//game controller
	GameControl game;

	//event manager
	std::vector<EventManager::StateEvent> events;

	//menu manager
	MenuHandler menus = MenuHandler(nullptr, -1, -1);

	/*
		Graphics and gui
	*/
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);	//background color
	bool Running = true;	//run flag
	bool debug = false;		//debug flag

	//game running flag
	bool draw_game = false;
	bool exit_flag = false;

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

	void onEvent(EventManager::StateEvent);

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

public:
	MainController();
	int Start();
};

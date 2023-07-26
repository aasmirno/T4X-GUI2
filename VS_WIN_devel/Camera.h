#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <assert.h>
#include <cmath>
#include <stdio.h>
#include <iostream>
#include <gl/glew.h>
#include <SDL.h>

/*
	Game camera class
	Handles:
		zooming
		panning
			mouse
			keyboard
*/
class Camera {
private:
	const float MIN_TRANSFORM = 1.0f;	//max zoom level
	const float MAX_TRANSFORM = 6.0f;	//min zoom level
	const float zoom_rate = 5.0f;		//zoom rate

	GLint transform_loc = -1;	//gl handle for projection matrix uniform
	
	float current_transform = 32.0f;	//current zoom level: starts at 32.0

	float cam_x = 0.0f;
	float cam_y = 0.0f;
	float cam_rate = 0.07f;

	//inner gl uniform update
	void updateProjection();

public:
	void reset();
	
	//coordinate helpers
	/*
		win_coord: window coordinate
		win_scale: window size axis
	*/
	float windowToCam(int win_coord, int win_scale) {
		return (float)(win_coord - (win_scale / 2)) / (win_scale / 2);
	}

	int windowToTileX(int win_coord, int win_scale) {
		//constants calibrated on 6000 tile map
		return floor((windowToCam(win_coord, win_scale) - cam_x) / (0.001562 * current_transform));
	}

	int windowToTileY(int win_coord, int win_scale) {
		//constants calibrated on 6000 tile map
		return floor((windowToCam(win_coord, win_scale) - cam_y) / (0.002778 * current_transform));
	}

	//void move functions
	void move(SDL_Event& event);

	//high level zoom function
	void zoomIn(float mouse_x, float mouse_y);
	void zoomOut();

	//getters
	float getX();
	float getY();
	float getWindowX(int window_width);
	float getWindowY(int window_height);

	float getZoom();	//returns current_transform

	/*
		Set transform uniform location
	*/
	void setTransformLoc(GLint location);
};
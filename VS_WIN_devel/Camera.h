#pragma once
#include <assert.h>
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
	const GLfloat MIN_TRANSFORM = 0.1f;	//max zoom level
	const GLfloat MAX_TRANSFORM = 6.0f;	//min zoom level
	const GLfloat zoom_rate = 0.3f;		//zoom rate

	GLint transform_loc = -1;
	GLfloat current_transform = 1.0f;	//current zoom level

	GLint cam_x = 0;
	GLint cam_y = 0;
	GLint cam_rate = 5;

	void updateTransform();

public:
	void move(SDL_Keycode key, int viewport_x, int viewport_y);

	//high level zoom function
	void zoomIn();
	void zoomOut();

	//getters
	GLint getX();
	GLint getY();
	GLfloat getZoom();	//returns current_transform

	/*
		Set transform uniform location
	*/
	void setTransformLoc(GLint location);
};
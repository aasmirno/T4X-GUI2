#pragma once
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
	const GLfloat zoomRate = 0.3f;		//zoom rate

	GLfloat current_transform = 1.0f;	//current zoom level

	GLint cam_x = 0;
	GLint cam_y = 0;
	GLint cam_rate = 5;

	void updateTransform(GLint transform_loc);

public:
	void move(SDL_Keycode key, int viewport_x, int viewport_y);

	void zoomIn(GLint transform_loc);
	void zoomOut(GLint transform_loc);

	GLint getX();
	GLint getY();
	GLfloat getZoom();

};
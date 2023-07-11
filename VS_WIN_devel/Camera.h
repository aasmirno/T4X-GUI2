#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>

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
	const GLfloat MIN_TRANSFORM = 5.0f;	//max zoom level
	const GLfloat MAX_TRANSFORM = 6.0f;	//min zoom level
	const GLfloat zoom_rate = 10.0f;		//zoom rate

	GLint transform_loc = -1;	//gl handle for projection matrix uniform
	
	GLfloat current_transform = 32.0f;	//current zoom level: starts at 32.0

	float cam_x = 0;
	float cam_y = 0;
	float cam_rate = 0.07f;

	//inner gl uniform update
	void updateProjection();

public:

	void move(SDL_Keycode key);

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
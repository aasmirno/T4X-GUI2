#pragma once
#include <stdio.h>
#include <iostream>
#include <gl/glew.h>
#include <SDL.h>

//game camera struct
class Camera {
private:
	const int cam_max = 10;
	const int cam_cool_max = 5;
	int cam_cool = 0;

	GLint cam_x = 0;
	GLint cam_y = 0;
	GLint cam_rate = 5;

public:
	void move(SDL_Keycode key);
	void zoomIn();
	void zoomOut();
	GLint getX();
	GLint getY();
};
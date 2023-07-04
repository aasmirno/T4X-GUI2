#include "Camera.h"

void Camera::move(SDL_Keycode key) {
	std::cout << cam_x << " " << cam_y << "\n";

	switch (key) {
	case SDLK_w:
		cam_y += cam_rate;
		break;
	case SDLK_s:
		cam_y -= cam_rate;
		break;
	case SDLK_a:
		cam_x -= cam_rate;
		break;
	case SDLK_d:
		cam_x += cam_rate;
		break;
	}
}

void Camera::zoomIn() {

}

void Camera::zoomOut() {

}

GLint Camera::getX() { return cam_x; };

GLint Camera::getY() { return cam_y; };
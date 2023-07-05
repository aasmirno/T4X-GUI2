#include "Camera.h"

GLint Camera::getX() { return cam_x; }
GLint Camera::getY() { return cam_y; }
GLfloat Camera::getZoom(){ return current_transform; }

void Camera::move(SDL_Keycode key, GLint viewport_x, GLint viewport_y) {

	switch (key) {
	case SDLK_w:
		cam_y -= cam_rate;
		break;
	case SDLK_s:
		cam_y += cam_rate;
		break;
	case SDLK_a:
		cam_x += cam_rate;
		break;
	case SDLK_d:
		cam_x -= cam_rate;
		break;
	default:
		printf("bad key\n");
		break;
	}

	glViewport(cam_x, cam_y, viewport_x, viewport_y);
}

void Camera::zoomIn(GLint transform_loc) {
	current_transform += zoomRate;
	updateTransform(transform_loc);
}

void Camera::zoomOut(GLint transform_loc) {
	current_transform -= zoomRate;
	updateTransform(transform_loc);
}

void Camera::updateTransform(GLint transform_loc) {
	if (current_transform < MIN_TRANSFORM) {
		current_transform = MIN_TRANSFORM;
	}

	if (current_transform > MAX_TRANSFORM) {
		current_transform = MAX_TRANSFORM;
	}

	glUniform1f(transform_loc, current_transform);
}
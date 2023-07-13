#include "Camera.h"

void Camera::setTransformLoc(GLint location) {
	transform_loc = location;
	assert(location != -1);
};

float Camera::getX() { return cam_x; }
float Camera::getY() { return cam_y; }
float Camera::getZoom() { return current_transform; }

void Camera::move(SDL_Keycode key) {

	switch (key) {
	case SDLK_w:
		cam_y += cam_rate;
		break;
	case SDLK_s:
		cam_y -= cam_rate;
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

	updateProjection();
}

void Camera::reset() {
	cam_x = 0.0f;
	cam_y = 0.0f;
	updateProjection();
}

void Camera::zoomIn() {
	current_transform += zoom_rate;
	cam_x -= 0.3;
	cam_y -= 0.3;
	updateProjection();
}

void Camera::zoomOut() {
	current_transform -= zoom_rate;
	cam_x += 0.3;
	cam_y += 0.3;

	if (current_transform < MIN_TRANSFORM) {
		current_transform = MIN_TRANSFORM;
		cam_x -= 0.3;
		cam_y -= 0.3;

	}
	updateProjection();
}

void debug_matrix(glm::mat4& mat) {
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			printf("%f ", mat[x][y]);
		}
		printf("\n");
	}
}

void Camera::updateProjection() {

	assert(transform_loc != -1); //make sure transform uniform location has been set

	glm::mat4 base = glm::translate(glm::mat4(1.0f), glm::vec3(cam_x, cam_y, 0.0f));
	glm::mat4 screen_resolution = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f / 1280.0f, 2.0f / 720.0f, 0.0f));
	glm::mat4 zoom = glm::scale(glm::mat4(1.0f), glm::vec3(current_transform, current_transform, 0.0f));	//generate zoom matrix
	debug_matrix(base);
	base = base * zoom * screen_resolution;

	glUniformMatrix4fv(transform_loc, 1, false, &base[0][0]);
}
#include "Camera.h"

void Camera::setTransformLoc(GLint location) {
	transform_loc = location;
	assert(location != -1);
};

float Camera::getX() { return cam_x; }
float Camera::getY() { return cam_y; }

float Camera::getWindowX(int window_width) {
	return (cam_x * window_width / 2) + window_width / 2;
}

float Camera::getWindowY(int window_height) {
	return (cam_y * window_height / 2) + window_height / 2;
}

float Camera::getZoom() { return current_transform; }

void Camera::move(SDL_Event& event) {

	if (event.type == SDL_MOUSEMOTION) {
		cam_x += event.motion.xrel * 0.001;
		cam_y += event.motion.yrel * 0.001;
	}
	else if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
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
	}

	updateProjection();
}

void Camera::reset(int x, int x_scale, int y, int y_scale) {
	cam_x = windowToCam(x, x_scale);
	cam_y = windowToCam(y, y_scale);
	current_transform = MIN_TRANSFORM;

	updateProjection();
}

void Camera::zoomIn(float mouse_x, float mouse_y) {
	current_transform += zoom_rate;
	cam_x -= mouse_x;
	cam_y -= mouse_y;
	updateProjection();
}

void Camera::zoomOut() {
	current_transform -= zoom_rate;
	if (current_transform < MIN_TRANSFORM) {
		current_transform = MIN_TRANSFORM;
	}
	else {
		current_transform -= zoom_rate;
		cam_x += 0.00781 * zoom_rate;
		cam_y += 0.02778 * zoom_rate;
		updateProjection();
	}
}

void debug_matrix(glm::mat4& mat) {
	glm::mat4 inv = mat;
	glm::inverse(inv);
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			printf("%f ", inv[x][y]);
		}
		printf("\n");
	}

}

void Camera::updateProjection() {

	assert(transform_loc != -1); //make sure transform uniform location has been set

	glm::mat4 base = glm::translate(glm::mat4(1.0f), glm::vec3(cam_x, cam_y, 0.0f));
	glm::mat4 screen_resolution = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f / 1280.0f, 2.0f / 720.0f, 0.0f));
	glm::mat4 zoom = glm::scale(glm::mat4(1.0f), glm::vec3(current_transform, current_transform, 0.0f));	//generate zoom matrix
	//debug_matrix(base);
	base = base * zoom * screen_resolution;

	glUniformMatrix4fv(transform_loc, 1, false, &base[0][0]);
}
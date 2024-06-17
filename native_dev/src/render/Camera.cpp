#include "T4X/render/Camera.h"

void Camera::translate(glm::vec3 unit_vec){
    view_matrix = glm::translate(view_matrix, unit_vec * speed_factors);
}

void Camera::initialise(){
}

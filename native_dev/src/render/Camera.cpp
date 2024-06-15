#include "render/Camera.h"

void Camera::updateCamera(SDL_KeyCode key){
    if(SDLK_UP){
        view_matrix = glm::translate(view_matrix, glm::vec3(0.0f,0.0f,-0.1f));
    }
    if(SDLK_DOWN){
        view_matrix = glm::translate(view_matrix, glm::vec3(0.0f,0.0f,0.1f));
    }
    if(SDLK_w){
        view_matrix = glm::translate(view_matrix, glm::vec3(0.0f,0.2f,0.0f));
    }
    if(SDLK_a){
        view_matrix = glm::translate(view_matrix, glm::vec3(-0.2f,0.0f,0.0f));
    }
    if(SDLK_s){
        view_matrix = glm::translate(view_matrix, glm::vec3(0.0f,-0.2f,0.0f));
    }
    if(SDLK_d){
        view_matrix = glm::translate(view_matrix, glm::vec3(0.2f,0.0f,0.0f));
    }
}

void Camera::scale(int magnitude){

}   



void Camera::initialise(){
}

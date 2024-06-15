#pragma once
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/mat4x4.hpp>
#include <glm/ext.hpp>

class Camera{
private:
    // camera view matrix
    glm::mat4 view_matrix = glm::mat4(1.0f);

    void scale(int magnitude);

public:
    GLfloat* getView(){
        return &view_matrix[0][0];
    }

    void updateCamera(SDL_KeyCode key);
    
    void initialise();

};
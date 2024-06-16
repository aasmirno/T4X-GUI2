#pragma once
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/mat4x4.hpp>
#include <glm/ext.hpp>

enum AXIS{
    X,
    Y,
    Z
};

class Camera{
private:
    // camera view matrix
    glm::mat4 view_matrix = glm::mat4(1.0f);

    glm::vec3 speed_factors = glm::vec3(0.01f, 0.01f, 0.1f);

public:
    GLfloat* getView(){
        return &view_matrix[0][0];
    }

    void setSpeed(AXIS a, float new_speed){
        switch (a)
        {
        case AXIS::Z:
            speed_factors[2] = new_speed;
            break;
        case AXIS::X:
            speed_factors[0] = new_speed;
            break;
        case AXIS::Y:
            speed_factors[1] = new_speed;
            break;
        default:
            break;
        }
    }

    void translate(glm::vec3 unit_vec);

    void initialise();

};
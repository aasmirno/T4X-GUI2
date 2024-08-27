#pragma once
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/mat4x4.hpp>
#include <glm/ext.hpp>

#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

enum AXIS{
    X,
    Y,
    Z
};

class Camera{
private:
    // camera vectors
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 4.0f);       // camera postion in world space
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);   // camera front vector
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);             // world space up

    float yaw = -90.0f; float pitch = 0.0f;

    //glm::mat4 view_matrix = glm::lookAt(position, position + cameraFront, up);
    glm::mat4 view_matrix = glm::mat4(1.0f);

    glm::vec3 speed_factors = glm::vec3(0.1f, 0.1f, 0.1f);

public:
    GLfloat* getView(){
        view_matrix = glm::lookAt(position, position + cameraFront, up);
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

    void moveYaw(float delta) {
        yaw += delta;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);
    }

    void movePitch(float delta) {
        pitch += delta;
        
        // pitch limits
        if (pitch > 89.0f)s
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);
    }

    void in() {
        position += cameraFront * speed_factors;
    }

    void out() {
        position -= cameraFront * speed_factors;
    }

    void translate(glm::vec3 unit_vec) {
        position += unit_vec * speed_factors;
        std::cout << "cam_pos " << glm::to_string(position) << "\n";
    }

    void left() {
        position -= glm::normalize(glm::cross(cameraFront, up)) * speed_factors;
    }

    void right() {
        position += glm::normalize(glm::cross(cameraFront, up)) * speed_factors;
    }

    /*
        move camera in the positive z direction (global)
    */
    void up_z() {
        position -= glm::vec3(0.0, 0.0 ,1.0) * speed_factors;
    }

    /*
        move camera in the positive z direction (global)
    */
    void down_z() {
        position += glm::vec3(0.0, 0.0, 1.0) * speed_factors;
    }

    void rotate(float angle, glm::vec3 unit_vec){
        view_matrix = glm::rotate(view_matrix, angle, unit_vec);
    }

    void initialise();

};
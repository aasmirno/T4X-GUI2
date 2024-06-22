#pragma once

#include "T4X/render/RenderObjects/RenderObject.h"

/*
	Testing renderable
*/
class TestObject : public RenderObject {
private:
    /*
        Object parameters
    */
    static const int NUM_SHADERS = 2;
	glm::mat4 model = glm::mat4(1.0f);

    /*
        Parent class init pipeline methods
    */
    virtual bool loadShaders();
    virtual bool setAttribs();
    virtual bool loadUniforms();

    virtual bool updateBuffers(int size, float* data);

    /*
        Hardcoded test data
    */
    static const int VERTICES = 9;
    float vertices[VERTICES] = {
        -0.5, -0.5, 0.5,
        0.5, -0.5, 0.0,
        0, 0.5, 0.0
    };

public:
    /*
        Print debug info for this object
    */
    virtual void printDebug();

    /*
        Draw this object
    */
    virtual void draw();

};
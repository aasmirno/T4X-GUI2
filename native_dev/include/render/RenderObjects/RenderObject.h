#pragma once
#include <GL/glew.h>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

#include <stdio.h>
#include <iostream>
#include <vector>

#include "render/Texture.h"
#include "render/Shader.h"

/*
    Base class for render objects
        object_id: id used by render manager for organisation
        shader_pid: id provided at initialisation, specifies handle of shader program to be used for drawing
        vao_id: vertex array object handle
        vbo_id: vertex_buffer object handle
*/
class RenderObject
{
protected:
    int object_id = -1; // render object id

    /*
        GL handles
    */
    GLuint vao_id = -1;    // vertex array object handle
    GLuint vbo_id = -1;    // vertex buffer object handle

    /*
        Generate the appropriate buffers for this object, implemented by sub class
    */
    virtual bool genBuffers() = 0;

    /*
        Update data in buffer
    */
    virtual bool updateBuffers() = 0;

public:
    /*
        Initialise the render object
            obj_id: render object id
    */
    bool initialise(int obj_id);

    /*
        Dump debug information to the console
    */
    virtual void printDebug() = 0;

    /*
        Draw method for a render object, implemented by sub class
    */
    virtual bool draw() = 0;
};
#pragma once
#include <GL/glew.h>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

#include <stdio.h>
#include <iostream>
#include <vector>

#include "T4X/render/Texture.h"
#include "T4X/render/Shader.h"
#include "T4X/tdefs.h"

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
        GL attributes
    */
    GLuint vao_id = -1; // vertex array object handle
    GLuint vbo_id = -1; // vertex buffer object handle
    Shader shader;      // shader program for this object

    /*
        General uniforms
    */
    GLint projection_location = -1; // projection matrix
    GLint view_location = -1;       // view matrix

    /*
        Generate the appropriate buffers for this object, implemented by sub class
    */
    virtual bool genBuffers() = 0;

    /*
        Update data in buffer
    */
    virtual bool updateBuffers(int size, float* data) = 0;

    /*
        Check GL error
    */
    bool checkGLError()
    {
        GLenum err;
        if ((err = glGetError()) != GL_NO_ERROR)
        {
            printf("[ RENDER OBJECT ERROR ] gl error: %d\n", err);
            return false;
        }
        return true;
    }

public:
    /*
        Initialise the render object
            obj_id: render object id
    */
    bool initialise(int obj_id)
    {
        object_id = obj_id;
        /*
            Generate buffers and set buffer parameters
        */
        glGenBuffers(1, &vbo_id);      // generate a vertex buffer object
        glGenVertexArrays(1, &vao_id); // gen a vertex array object
        if (vbo_id == -1 || vao_id == -1)
        {
            printf("buffer generation error: vbo_id=%d vao_id=%d, obj_id=%d\n", vbo_id, vao_id, obj_id);
            return false;
        }

        // call subclass specific methods
        return genBuffers();
    }

    /*
        Dump debug information to the console
    */
    virtual void printDebug() = 0;

    /*
        Draw method for a render object, implemented by sub class
    */
    virtual void draw() = 0;

    /*
        Set view matrix
            GLfloat *matrix: pointer to mat4 transform matrix
            std::string type (view/projection): specified uniform to update
    */
    bool setTransform(GLfloat *matrix, std::string type)
    {
        // check shader init
        if (shader.program_id == 0)
        {
            printf("[ RENDER OBJECT ERROR ] uninitialised shader\n");
            return false;
        }

        // check proper type string
        if (type != "projection" && type != "view")
        {
            printf("[ RENDER OBJECT ERROR ] improper type: %s, need view or projection\n", type);
            return false;
        }

        // check locations defined and set locations
        GLint location = -1;
        if (type == "view" && view_location == -1)
        {
            printf("[ RENDER OBJECT ERROR ] no view location defined\n");
            return false;
        }
        else
        {
            location = view_location;
        }

        if (type == "projection" && projection_location == -1)
        {
            printf("[ RENDER OBJECT ERROR ] no projection location defined\n");
            return false;
        }
        else
        {
            location = projection_location;
        }

        // redundant check
        if (location == -1)
        {
            printf("[ RENDER OBJECT ERROR ] unknown setTransform error, location = -1\n");
            return false;
        }

        // update transforms
        glUseProgram(shader.program_id);
        glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
        if (!checkGLError())
        {
            printf("[ RENDER OBJECT ERROR ] error in %s update\n", type);
            printDebug();
            return false;
        }
        return true;
    }
};
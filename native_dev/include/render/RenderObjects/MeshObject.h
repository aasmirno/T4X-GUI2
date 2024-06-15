#pragma once
#include "render/RenderObjects/RenderObject.h"

class MeshObject : public RenderObject
{
private:
    /*
        Implementation of Render Object buffer generation function
    */
    virtual bool genBuffers();
    virtual bool updateBuffers();

    GLint projection_location = -1; // projection uniform location


    const uint NUM_SHADERS = 4;
    int num_vert = 15;
    GLfloat test_vertices[15] = {
        -0.5f, 0.5f, 0.0f,  // tl
        -0.5f, -0.5f, 0.0f, // bl
        0.5f, 0.5f, 0.0f,   // tr
        0.5f, -0.5f, 0.0f,  // br
        0.7f, 0.7f, 0.0f};

    std::vector<GLfloat> vertices;

    float width = 100;   //mesh width
    float heigth = 100;  //mesh height
    unsigned patch_resolution = 10; //tesselation patch resolution

public:
    bool setTransform(GLfloat *transform);

    /*
        Print debug info for this object
    */
    virtual void printDebug();

    /*
        Draw this object
    */
    virtual void draw();
};
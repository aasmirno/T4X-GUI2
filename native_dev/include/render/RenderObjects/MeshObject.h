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

    // shader uniform locations
    GLint projection_location = -1; // projection matrix
    GLint model_location = -1;      // model matrix
    GLint view_location = -1;      // view matrix

    const uint NUM_SHADERS = 4;

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);

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
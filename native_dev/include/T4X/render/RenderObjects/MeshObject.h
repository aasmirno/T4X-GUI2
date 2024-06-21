#pragma once
#include "T4X/render/RenderObjects/RenderObject.h"
#include "T4X/tdefs.h"

/*
    Implementation of Render Object buffer generation function
*/
class MeshObject : public RenderObject
{
private:
    virtual bool genBuffers();
    bool updateBuffers(int size, float* data);

    // shader uniform locations
    GLint model_location = -1;      // model matrix
    const uint NUM_SHADERS = 4;     // number of shaders
    unsigned patch_resolution = 0;

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
public:
    /*
        Print debug info for this object
    */
    virtual void printDebug();

    /*
        Draw this object
    */
    virtual void draw();

    /*
        Set mesh data
    */
    bool setMeshData(std::vector<float> data, unsigned patches);
};
#pragma once
#include "T4X/render/Texture.h"
#include "T4X/render/RenderObjects/RenderObject.h"
#include "T4X/tdefs.h"

/*
    Implementation of Render Object buffer generation function
*/
class MeshObject : public RenderObject
{
private:
    /*
        Object parameters
    */
    const uint NUM_SHADERS = 4;     // number of shaders
    glm::mat4 model = glm::mat4(1.0f);
    RenderTexture t;

    /*
        Parent class init pipeline methods
    */
    virtual bool loadShaders();
    virtual bool setAttribs();
    virtual bool loadUniforms();

    virtual bool updateBuffers(int size, float* data);

    // class specific parameters
    unsigned patch_resolution = 0;
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
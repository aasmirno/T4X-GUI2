#pragma once
#include "T4X/render/RenderObjects/WorldObjects/WorldObject.h"

/*
    Implementation of Render Object buffer generation function
*/
class MeshObject : public WorldObject
{
private:
    /*
        Object parameters
    */
    const uint NUM_SHADERS = 4;         // number of shaders
    glm::mat4 model = glm::mat4(1.0f);  // model matrix
    RenderTexture t;                    // object textures

    int height = 50; int width = 50;
    int resolution = 10;

    /**************************************
        Parent class init pipeline methods
    ***************************************/

    /*
        Load Mesh shader program and load uniforms
    */
    virtual bool loadShaders();
    virtual bool setAttribs();

    virtual bool updateBuffers(int size, float* data);
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
        Cleanup textures
    */
    virtual void cleanup();

    /*
        Set mesh data
        - generate vertex map from map data
        - generate normals
        - push to buffers
        args:
        - float* data: pointer to height data
        - unsigned size: size of data
    */
    bool setMeshData(float* data, unsigned size);
};
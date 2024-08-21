#pragma once
#include "T4X/render/Textures/ImageTexture.h"
#include "T4X/render/Textures/DataTexture.h"
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
    ImageTexture color_texture;
    ImageTexture mesh_texture;

    int height = 200; int width = 200;
    GLint height_location;
    GLint width_location;
    int resolution = 100;

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
    bool setMeshData(float* data, int data_width, int data_height);
};
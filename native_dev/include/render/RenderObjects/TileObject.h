#pragma once
#include "render/RenderObjects/RenderObject.h"

class TileObject : public RenderObject
{
private:
    GLint transform_loc = -1; // transform matrix uniform location
    GLint dimension_loc = -1; // tile dimension uniform location
    GLint offset_loc = -1;    // position offset uniform location

    glm::ivec2 dimensions{1, 1};  // tile data dimensions
    glm::vec2 offset{0.0f, 0.0f}; // tile data dimensions

    uint16_t *data; // pointer to data

    GLTexture texture;  // texture for this object
    Shader shader_prog; // shader program for this object

    /*
        Implementation of Render Object buffer generation function
    */
    virtual bool genBuffers();

    /*
        Implementation of Render Object buffer update function
            Sends vertex data into vertex buffer
    */
    virtual bool updateBuffers();

    /*
        Set the dimensions for this object
    */
    bool setDims(int x_dim, int y_dim);

    /*
        Check for object initialisation
    */
    bool checkInit();

public:
    /*
        Cleanup functions to delete shaders and texture for this object
    */
    bool cleanup();

    /*
        Implementation of Render Object draw function, draws vertices in vertex_data
    */
    virtual bool draw();

    /*
        Set the texture for this object
    */
    bool setTexture(const char *filename, unsigned w, unsigned h);

    /*
        update tile data
    */
    bool setData(uint16_t *new_data, int x_dim, int y_dim);

    /*
        Set transform matrix for this object
    */
    bool update_transform(GLfloat *transform);

    bool setOffset(GLfloat x_off, GLfloat y_off);

    /*
        Print debug information
    */
    virtual void printDebug();
};
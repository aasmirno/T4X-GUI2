#pragma once
#include "render/RenderObjects/RenderObject.h"
#include "render/Texture.h"

#include <glm/vec2.hpp>

class TileObject : public RenderObject
{
private:
    GLint transform_loc = -1; //transform matrix uniform location
    GLint dimension_loc = -1; //tile dimension uniform location

    glm::ivec2 dimensions{1,1};      //tile data dimensions
    
    uint16_t* data; //pointer to data
    GLTexture texture;  //textures

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
    void setDims(int x_dim, int y_dim);

public:
    /*
        Implementation of Render Object draw function, draws vertices in vertex_data
    */
    virtual bool draw();

    /*
        Set the texture for this object
    */
    bool setTexture(const char* filename, unsigned w, unsigned h);

    /*
        update tile data
    */
    void setData(uint16_t* new_data, int x_dim, int y_dim);

    /*
        Set transform matrix for this object
    */
    void update_transform(GLfloat* transform);

    virtual void printDebug();
};
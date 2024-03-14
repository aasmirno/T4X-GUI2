#pragma once
#include "render/RenderObjects/RenderObject.h"
#include "render/Texture.h"

#include <glm/vec2.hpp>

/*
    TODO:
        add data interaction methods
        add texturing
*/

class TileObject : public RenderObject
{
private:
    GLint transform_loc = -1; //transform matrix uniform location
    GLint dimension_loc = -1; //tile dimension location

    glm::ivec2 dimensions{1,1};
    std::vector<uint16_t> tile_data; //store tile data
    
    GLTexture texture;

    /*
        Implementation of Render Object buffer generation function
    */
    virtual bool genBuffers();

    /*
        Implementation of Render Object buffer update function
            Sends vertex data into vertex buffer
    */
    virtual bool updateBuffers();

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
        Set the dimensions for this object
    */
    void setDims(int x_dim, int y_dim);

    void update_transform(GLfloat* transform);

    virtual void printDebug();
};
#pragma once
#include "RenderObjects/RenderObject.h"
#include <vector>

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

    GLint dim_x = 1;
    GLint dim_y = 1;
    std::vector<uint16_t> tile_data; //store tile data
    
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

    void update_transform(GLfloat* transform);

    virtual void printDebug();
};
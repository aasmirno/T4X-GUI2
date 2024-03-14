#pragma once
#include "render/RenderObjects/RenderObject.h"

class ShapeObject : public RenderObject
{
private:
    std::vector<float> vertex_data; // store vertex information for this object

    GLint color_loc = -1;
    glm::vec4 shape_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

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
        add a point into the vertex data array
    */
    void addVertex(float x, float y, float z)
    {
        vertex_data.push_back(x);
        vertex_data.push_back(y);
        vertex_data.push_back(z);
        updateBuffers();
    }

    virtual void printDebug();

    void setColor(glm::vec4 color)
    {
        shape_color.x = color.x;
        shape_color.y = color.y;
        shape_color.z = color.z;
        shape_color.w = color.w;
    }

    /*
        remove point from the vertex data array
    */
    void removeVertex(float x, float y, float z)
    {
        vertex_data.pop_back();
        updateBuffers();
    }

    /*
        Implementation of Render Object draw function, draws vertices in vertex_data
    */
    virtual bool draw();
};
#pragma once
#include "RenderObject.h"
#include <vector>

class ShapeObject : public RenderObject{
    private:
        std::vector<float> vertex_data; //store vertex information for this object
        
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
        void addVertex(float x, float y, float z){
            vertex_data.push_back(x);
            vertex_data.push_back(y);
            vertex_data.push_back(z);
            updateBuffers();
        }

        /*
            remove point from the vertex data array
        */
        void removeVertex(float x, float y, float z){
            vertex_data.pop_back();
            updateBuffers();
        }

        /*
            Implementation of Render Object draw function, draws vertices in vertex_data
        */
        virtual bool draw();
};
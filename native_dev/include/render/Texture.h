#pragma once
#define GL_GLEXT_PROTOTYPES
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <PNG/lodepng.h>

#include <vector>

class GLTexture
{
private:
    GLuint handle = 0;               // texture handle
    int width = -1;                  // texture width
    int height = -1;                 // texture height
    std::vector<unsigned char> data; // Image data RGBA format

public:
    /*
        Get size of image data array
    */
    size_t size(){
        return data.size();
    }

    /*
        Check for initialisation
    */
    bool active(){
        return (handle != 0 && width != 0 && height != 0 && data.size() != 0);
    }

    /*
        load a texture from a png file
    */
    bool createTexture(const char *filename, unsigned w, unsigned h);

    bool deleteTexture();

    /*
        load a texture into the sampler
    */
    bool setActive();
};
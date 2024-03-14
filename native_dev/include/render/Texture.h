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
    GLuint handle = -1;              // texture handle
    int width = -1;                  // texture width
    int height = -1;                 // texture height
    std::vector<unsigned char> data; // Image data RGBA format

public:
    /*
        load a texture form a png file
    */
    bool initialise(const char *filename, unsigned w, unsigned h);

    /*
        load a texture into the sampler
    */
    bool load_texture();
};
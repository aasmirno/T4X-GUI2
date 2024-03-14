#include "render/Texture.h"

bool GLTexture::initialise(const char *filename, unsigned w, unsigned h)
{
    //reset texture data
    handle = 0;
    width = 0;
    height = 0;
    data.clear();

    if (w == 0 || h == 0)
    {
        printf("ERROR: improper dimensions w=%d, h=%d\n", w, h);
        return false;
    }

    // create a new texture struct and load in png data
    width = w;
    height = h;
    unsigned error = lodepng::decode(data, w, h, filename);
    if (error)
    {
        printf("ERROR: lodepng error=%s\n", lodepng_error_text(error));
        return false;
    }

    // generate opengl texture and set handle
    GLuint texture_handle = 0;
    glGenTextures(1, &texture_handle);
    if (texture_handle == 0)
    {
        printf("ERROR: texture gen error\n");
        return false;;
    }
    handle = texture_handle;
    return true;
}

bool GLTexture::load_texture(){
    // error checking
        if (handle == -1)
        {
            printf("ERROR: tried to load unitialised texture\n");
            return false;
        }

        // load the texture into the opengl sampler and format
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        return true;
}
#include "render/RenderObjects/RenderObject.h"

/*
    Renderable object representing a textured rectangle
*/
class TexturedObject : public RenderObject
{
    GLint transform_loc = -1; // transform matrix uniform location
    GLint height_loc = -1;    // height uniform location
    GLint width_loc = -1;     // width uniform location

    GLfloat origin[2] = {0.0f, 0.0f}; // origin for this object
    GLfloat height = 100.0f;
    GLfloat width = 100.0f; // height and width for this object

    GLTexture texture; // texture for this object
private:
    /*
        Implementation of Render Object buffer generation function
    */
    virtual bool genBuffers();

    /*
        udateBuffers:
        Push current object attributes to gl buffer
    */
    virtual bool updateBuffers();

public:
    /*
        Getters
    */
    GLfloat *getOrigin() { return &origin[0]; }
    GLfloat getHeight() { return height; }
    GLfloat getWidth() { return width; }

    /*
        Print debug info for this object
    */
    virtual void printDebug();

    /*
        Draw this object
    */
    virtual void draw();

    /*
        Set texture for this object
            const char *filename: relative filepath for texture source
            unsigned w, h: texture dimensions
    */
    bool setTexture(const char *filename, unsigned w, unsigned h);

    /*
        update transform for this object
            GLfloat* transform: pointer to [0][0] of a 4x4 transform matrix
    */
    bool setTransform(GLfloat *transform);

    /*
        update origin for this object
            GLfloat x,y: x and y floats for bottom right corner of the texture
    */
    bool setOrigin(GLfloat x, GLfloat y);

    /*
        Update height or width
            GLfloat new_height/width: new heigth/width value
    */
    bool setHeigth(GLfloat new_height);
    bool setWidth(GLfloat new_width);

    /*
        Destroy any memory using object belonging to this instance
    */
    bool cleanup()
    {
        shader.deleteProgram(); // delete shaders
        texture.deleteTexture();     // delete texture
        return true;
    }
};
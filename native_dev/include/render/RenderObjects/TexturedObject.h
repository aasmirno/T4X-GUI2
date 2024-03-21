#include "render/RenderObjects/RenderObject.h"


/*
    Renderable object representing a textured rectangle
*/
class TexturedObject : public RenderObject{
    GLint transform_loc = -1; // transform matrix uniform location

    GLfloat origin[3] = {0.0f ,0.0f, 0.0f};   //origin for this object
    float height = 100;   float width = 100;    //height and width for this object


    GLTexture texture;  // texture for this object
    Shader shader_prog; // shader program for this object
private:
    /*
        Implementation of Render Object buffer generation function
    */
    virtual bool genBuffers();
    virtual bool updateBuffers();

public:

    /*
        Print debug info for this object
    */
    virtual void printDebug();

    /*
        Draw this object
    */
    virtual bool draw();

    /*
        Set texture for this object
    */
    bool setTexture(const char *filename, unsigned w, unsigned h);

    /*
        update transform for this object
    */
    bool update_transform(GLfloat *transform);

    /*
        Destroy any memory using object belogning to this instance
    */
    bool cleanup(){
        shader_prog.deleteProgram();    //delete shaders
        texture.deleteTexture();        //delete texture
        return true;
    }

};
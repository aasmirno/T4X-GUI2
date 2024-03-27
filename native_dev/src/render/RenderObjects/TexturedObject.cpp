#include "render/RenderObjects/TexturedObject.h"

void TexturedObject::printDebug(){
    printf("DEBUG: info for TextureObject %d\n", object_id);
    printf("    shader_pid: %d\n", shader_prog.program_id);
    printf("    uniform locations: transform=%d\n", transform_loc);
    printf("    texture size: %d\n", texture.size());
}

bool TexturedObject::genBuffers(){
    /*
        Generate buffers and set buffer parameters
    */
    glGenBuffers(1, &vbo_id);           //generate a vertex buffer object
    glGenVertexArrays(1, &vao_id);      //gen a vertex array object
    if (vbo_id == -1 || vao_id == -1)
    {
        printf("buffer generation error: vbo_id=%d vao_id=%d \n", vbo_id, vao_id);
        return false;
    }

    glBindVertexArray(vao_id);    // focus va buffer
    glEnableVertexAttribArray(0); // enable attrib array at 0
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id); // focus vb
    /*
        Format for input vertices
            location: 0
            size: 3
            type: float
    */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    /*
        Initialise Shaders
    */
   SourcePair texture_program[] = {   
            SourcePair{"resources/textured_shaders/texturevert.glvs", GL_VERTEX_SHADER},
            SourcePair{"resources/textured_shaders/texturegeom.glgs", GL_GEOMETRY_SHADER},
            SourcePair{"resources/textured_shaders/texturefrag.glfs", GL_FRAGMENT_SHADER}};
    shader_prog.createProgram(&texture_program[0], 3);

    //get uniform locations
    transform_loc = glGetUniformLocation(shader_prog.program_id, "projection");
    if (transform_loc == -1)
    {
        printf("could not find uniform %s render_obj=%d\n", "projection", object_id);
        printDebug();
        return false;
    }

    updateBuffers();

    // check gl errors
    GLenum err;
    if ((err = glGetError()) != GL_NO_ERROR)
    {
        printf("ERROR: gl error in buffer generation: glenum=%d\n", err);
        printDebug();
        return false;
    }
    return true;
}

bool TexturedObject::updateBuffers(){
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float), &origin[0], GL_STATIC_DRAW);
    return true;
}

bool TexturedObject::update_transform(GLfloat *transform)
{
    glUseProgram(shader_prog.program_id);
    glUniformMatrix4fv(transform_loc, 1, GL_FALSE, transform);
    return true;
}

bool TexturedObject::update_origin(GLfloat x, GLfloat y){
    origin[0] = x; origin[1] = y;
    return true;
}


bool TexturedObject::setTexture(const char* filename, unsigned w, unsigned h){
    return texture.createTexture(filename, w, h);
}


bool TexturedObject::draw()
{
    glUseProgram(shader_prog.program_id);  // set shader program
    glBindVertexArray(vao_id);             // bind vertex array
    texture.setActive();                   // load texture into sampler
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glDrawArrays(GL_POINTS, 0, 1);          //draw the object

    return true;
}


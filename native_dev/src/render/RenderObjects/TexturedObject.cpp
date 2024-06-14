#include "render/RenderObjects/TexturedObject.h"

void TexturedObject::printDebug()
{
    printf("DEBUG: info for TextureObject %d\n", object_id);
    printf("    shader_pid: %d\n", shader.program_id);
    printf("    vao_id: %d\n", vao_id);
    printf("    vbo_id: %d\n", vbo_id);
    printf("    uniform locations: transform=%d\n", transform_loc);
    printf("    texture size: %d\n", texture.size());
}

bool TexturedObject::genBuffers()
{
    glBindVertexArray(vao_id);    // focus va buffer
    glEnableVertexAttribArray(0); // enable attrib array at 0

    glBindBuffer(GL_ARRAY_BUFFER, vbo_id); // focus vb
    /*
        Format for input vertices
            location: 0
            size: 2
            type: float
    */
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    // initialise shaders
    SourcePair texture_program[] = {
        SourcePair{"resources/textured_shaders/texturevert.glvs", GL_VERTEX_SHADER},
        SourcePair{"resources/textured_shaders/texturegeom.glgs", GL_GEOMETRY_SHADER},
        SourcePair{"resources/textured_shaders/texturefrag.glfs", GL_FRAGMENT_SHADER}};
    bool shader_success = shader.createProgram(&texture_program[0], 3);
    if (!shader_success)
    {
        printDebug();
        printf("Shader Failure\n");
        return false;
    }

    // get uniform locations
    transform_loc = glGetUniformLocation(shader.program_id, "projection");
    if (transform_loc == -1)
    {
        printf("could not find uniform %s render_obj=%d\n", "projection", object_id);
        printDebug();
        return false;
    }

    height_loc = glGetUniformLocation(shader.program_id, "height");
    if (height_loc == -1)
    {
        printf("could not find uniform %s render_obj=%d\n", "height", object_id);
        printDebug();
        return false;
    }

    width_loc = glGetUniformLocation(shader.program_id, "width");
    if (height_loc == -1)
    {
        printf("could not find uniform %s render_obj=%d\n", "width", object_id);
        printDebug();
        return false;
    }

    // check gl errors
    if (!checkGLError())
    {
        printf("ERROR: gl error in buffer intialisation");
        printDebug();
        return false;
    }

    printDebug();
    return true;
}

bool TexturedObject::updateBuffers()
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float), &origin[0], GL_STATIC_DRAW);
    if (!checkGLError())
    {
        printf("ERROR: gl error in buffer update\n");
        printDebug();
        return false;
    }
    return true;
}

bool TexturedObject::setTransform(GLfloat *transform)
{
    glUseProgram(shader.program_id);
    glUniformMatrix4fv(transform_loc, 1, GL_FALSE, transform);
    if (!checkGLError())
    {
        printf("ERROR: gl error in transform update\n");
        printDebug();
        return false;
    }
    return true;
}

bool TexturedObject::setOrigin(GLfloat x, GLfloat y)
{
    origin[0] = x;
    origin[1] = y;
    return updateBuffers();
}

bool TexturedObject::setHeigth(GLfloat new_height)
{
    height = new_height;
    glUseProgram(shader.program_id);
    glUniform1f(height_loc, height);
    if (!checkGLError())
    {
        printf("ERROR: gl error in transform update\n");
        printDebug();
        return false;
    }
    return updateBuffers();
}

bool TexturedObject::setWidth(GLfloat new_width)
{
    width = new_width;
    glUseProgram(shader.program_id);
    glUniform1f(width_loc, width);
    if (!checkGLError())
    {
        printf("ERROR: gl error in transform update\n");
        printDebug();
        return false;
    }
    return updateBuffers();
}

bool TexturedObject::setTexture(const char *filename, unsigned w, unsigned h)
{
    return texture.createTexture(filename, w, h);
}

void TexturedObject::draw()
{
    glUseProgram(shader.program_id); // set shader program
    glBindVertexArray(vao_id);            // bind vertex array

    texture.setActive(); // load texture into sampler

    glDrawArrays(GL_POINTS, 0, 1); // draw the object
}

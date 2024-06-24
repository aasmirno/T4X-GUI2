#include "T4X/render/RenderObjects/TexturedObject.h"

void TexturedObject::printDebug()
{
    printf("DEBUG: info for TextureObject %d\n", object_id);
    printf("    shader_pid: %d\n", shader.program_id);
    printf("    vao_id: %d\n", vao_id);
    printf("    vbo_id: %d\n", vbo_id);
}

bool TexturedObject::setAttribs() {
    glBindVertexArray(vao_id);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    /*
        Format for input vertices
            location: 0
            size: 2
            type: float
    */
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    if (!checkGLError()) return false;
    return true;
}

bool TexturedObject::loadShaders() {
    // initialise shaders
    SourcePair texture_program[] = {
        SourcePair{"texturevert.glvs", GL_VERTEX_SHADER},
        SourcePair{"texturegeom.glgs", GL_GEOMETRY_SHADER},
        SourcePair{"texturefrag.glfs", GL_FRAGMENT_SHADER} };
    bool shader_success = shader.createProgram(&texture_program[0], 3);
    if (!shader_success)
    {
        printDebug();
        printf("Shader Failure\n");
        return false;
    }

    return true;
}

bool TexturedObject::loadUniforms() {
    projection_location = glGetUniformLocation(shader.program_id, "projection");
    if (projection_location == -1)
    {
        printf("could not find uniform %s render_obj=%d\n", "projection", object_id);
        printDebug();
        return false;
    }

    view_location = glGetUniformLocation(shader.program_id, "view");
    if (view_location == -1)
    {
        printf("could not find uniform %s render_obj=%d\n", "projection", object_id);
        printDebug();
        return false;
    }

    dimension_location = glGetUniformLocation(shader.program_id, "dimensions");
    if (dimension_location == -1)
    {
        printf("could not find uniform %s render_obj=%d\n", "dimensions", object_id);
        printDebug();
        return false;
    }

    updateBuffers(2 * sizeof(float), &origin[0]);
    return true;
}

bool TexturedObject::updateBuffers(int size, float *data)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    if (!checkGLError())
    {
        printf("[ TEXTURED OBJECT ] gl error in buffer update\n");
        printDebug();
        return false;
    }
    return true;
}

void TexturedObject::draw()
{
    glUseProgram(shader.program_id); // set shader program
    glBindVertexArray(vao_id);       // bind vertex array
    glBindTexture(GL_TEXTURE_2D, texture.handle);

    glDrawArrays(GL_POINTS, 0, 1); // draw the object
}

bool TexturedObject::loadTexture(const char* filename) {
    if (!texture.setTexture(filename)) {
        return false;
    }
    
    dimensions[0] = texture.width;
    dimensions[1] = texture.height;
    glUseProgram(shader.program_id); // set shader program
    glUniform2f(dimension_location, dimensions[0], dimensions[1]);

    return checkGLError();
}
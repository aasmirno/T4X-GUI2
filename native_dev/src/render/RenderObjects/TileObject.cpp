#include "render/RenderObjects/TileObject.h"

bool TileObject::draw()
{
    glUseProgram(shader_pid);  // set shader program
    glBindVertexArray(vao_id); // bind vertex array
    texture.load_texture();    // load texture into sampler

    glDrawArrays(GL_POINTS, 0, dimensions.x * dimensions.y); // Draw all points from the current vao with assigned shader program
    return true;
}

bool TileObject::updateBuffers()
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);                                                             // focus vbo for this render object
    glBufferData(GL_ARRAY_BUFFER, dimensions.x * dimensions.y * sizeof(uint16_t), data, GL_STATIC_DRAW); // insert data from vertex_data
    return true;
}

void TileObject::update_transform(GLfloat *transform)
{
    printf("0,0=%f\n", *transform);
    glUseProgram(shader_pid);
    glUniformMatrix4fv(transform_loc, 1, GL_FALSE, transform);
}

bool TileObject::genBuffers()
{
    glGenBuffers(1, &vbo_id);      // generate a vertex buffer object
    glGenVertexArrays(1, &vao_id); // gen a vertex array object
    if (vbo_id == -1 || vao_id == -1)
    {
        printf("buffer generation error: vbo_id=%d vao_id=%d \n", vbo_id, vao_id);
        return false;
    }

    glBindVertexArray(vao_id);    // focus va buffer
    glEnableVertexAttribArray(0); // enable attrib array at 0

    glBindBuffer(GL_ARRAY_BUFFER, vbo_id); // focus vb
    /*
        Format for input array
            location: 0
            size: 1
            type: GLuint
    */
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_SHORT, 0, 0);

    // Get uniform locations
    transform_loc = glGetUniformLocation(shader_pid, "projection");
    if (transform_loc == -1)
    {
        printf("could not find uniform %s render_obj=%d\n", "projection", object_id);
        printDebug();
        return false;
    }

    dimension_loc = glGetUniformLocation(shader_pid, "dimensions");
    if (dimension_loc == -1)
    {
        printf("could not find uniform %s render_obj=%d\n", "dimensions", object_id);
        printDebug();
        return false;
    }

    // check gl errors
    GLenum err;
    if ((err = glGetError()) != GL_NO_ERROR)
    {
        printf("buffer generation error: glenum=%d\n", err);
        printDebug();
        return false;
    }
    updateBuffers();
    return true;
}

bool TileObject::setTexture(const char* filename, unsigned w, unsigned h){
    texture.initialise(filename, w, h);
    printf("%d\n", texture.size());
    return true;
}

void TileObject::setDims(int x_dim, int y_dim)
{
    if (y_dim != x_dim)
    {
        printf("ERROR: mismatched dims for tile obj: x=%d, y=%d\n", x_dim, y_dim);
    }

    dimensions.x = x_dim;
    dimensions.y = y_dim;

    glUseProgram(shader_pid);                 // set shader program
    glUniform2iv(dimension_loc, 1, &dimensions.x); // set mapsize uniform
}

void TileObject::printDebug()
{
    printf("debug info for TileObject %d\n", object_id);
    printf("    shader_pid: %d\n", shader_pid);
}

void TileObject::setData(uint16_t* new_data, int x_dim, int y_dim){
    setDims(x_dim, y_dim);
    data = new_data;
    updateBuffers();
}
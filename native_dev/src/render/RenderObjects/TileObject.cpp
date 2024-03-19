#include "render/RenderObjects/TileObject.h"

bool TileObject::checkInit(){
    // //check texture
    // if(!texture.active()){
    //     printf("ERROR: texture not initialised:\n");
    //     printDebug();
    //     return false;
    // }

    // //check uniform locations
    // if(transform_loc == -1 || dimension_loc == -1){
    //     printf("ERROR: uniform locations not set:\n");
    //     printDebug();
    //     return false;
    // }

    // //check data
    // if(data == nullptr){
    //     printf("ERROR: data pointer not set:\n");
    //     printDebug();
    //     return false;
    // }

    return true;
}

bool TileObject::cleanup(){
    shader_prog.deleteProgram();    //delete shaders
    texture.deleteTexture();        //delete texture
    return true;
}

bool TileObject::draw()
{
    glUseProgram(shader_prog.program_id);  // set shader program
    glBindVertexArray(vao_id); // bind vertex array
    texture.setActive();    // load texture into sampler

    glDrawArrays(GL_POINTS, 0, dimensions.x * dimensions.y); // Draw all points from the current vao with assigned shader program
    return true;
}

bool TileObject::updateBuffers()
{
    if(!checkInit()){return false;}
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);                                                             // focus vbo for this render object
    glBufferData(GL_ARRAY_BUFFER, dimensions.x * dimensions.y * sizeof(uint16_t), data, GL_STATIC_DRAW); // insert data from vertex_data
    return true;
}

bool TileObject::genBuffers()
{
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
        Format for input array
            location: 0
            size: 1
            type: GLuint
    */
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_SHORT, 0, 0);



    /*
        Initialise Shaders
    */
   SourcePair tile_program[] = {   
            SourcePair{"resources/tiled_shaders/tilevert.glvs", GL_VERTEX_SHADER},
            SourcePair{"resources/tiled_shaders/tilegeom.glgs", GL_GEOMETRY_SHADER},
            SourcePair{"resources/tiled_shaders/tilefrag.glfs", GL_FRAGMENT_SHADER}};
    shader_prog.createProgram(&tile_program[0], 3);

    //get uniform locations
    transform_loc = glGetUniformLocation(shader_prog.program_id, "projection");
    if (transform_loc == -1)
    {
        printf("could not find uniform %s render_obj=%d\n", "projection", object_id);
        printDebug();
        return false;
    }

    dimension_loc = glGetUniformLocation(shader_prog.program_id, "dimensions");
    if (dimension_loc == -1)
    {
        printf("could not find uniform %s render_obj=%d\n", "dimensions", object_id);
        printDebug();
        return false;
    }

    offset_loc = glGetUniformLocation(shader_prog.program_id, "offset");
    if (offset_loc == -1)
    {
        printf("could not find uniform %s render_obj=%d\n", "offset", object_id);
        printDebug();
        return false;
    }

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


bool TileObject::update_transform(GLfloat *transform)
{
    if(!checkInit()){return false;}
    glUseProgram(shader_prog.program_id);
    glUniformMatrix4fv(transform_loc, 1, GL_FALSE, transform);
    return true;
}

bool TileObject::setTexture(const char* filename, unsigned w, unsigned h){
    return texture.createTexture(filename, w, h);
}

bool TileObject::setDims(int x_dim, int y_dim)
{
    if(!checkInit()){return false;}
    dimensions.x = x_dim;
    dimensions.y = y_dim;

    glUseProgram(shader_prog.program_id);          // set shader program
    glUniform2iv(dimension_loc, 1, &dimensions.x); // set mapsize uniform
    return true;
}

bool TileObject::setData(uint16_t* new_data, int x_dim, int y_dim){
    if(new_data == nullptr || x_dim < 1 || y_dim < 1){return false;}
    setDims(x_dim, y_dim);
    data = new_data;
    updateBuffers();
    return true;
}

bool TileObject::setOffset(GLfloat x_off, GLfloat y_off){
    offset.x += x_off;
    offset.y += y_off;
    glUseProgram(shader_prog.program_id);          // set shader program
    glUniform2f(offset_loc, offset.x, offset.y);   // update unif vals
    return true;
}

void TileObject::printDebug()
{
    printf("DEBUG: info for TileObject %d\n", object_id);
    printf("    shader_pid: %d\n", shader_prog.program_id);
    printf("    current dimensions: x=%d, y=%d\n", dimensions.x, dimensions.y);
    printf("    uniform locations: transform=%d, dims=%d\n", transform_loc, dimension_loc);
    printf("    data pointer: %d\n", data);
    printf("    texture size: %d\n", texture.size());
}
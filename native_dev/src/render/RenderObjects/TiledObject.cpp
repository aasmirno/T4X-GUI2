#include "RenderObjects/TileObject.h"

bool TileObject::draw(){
    glUseProgram(shader_pid);           // set shader program
    glBindVertexArray(vao_id);          // bind vertex array

    glUniform2i(dimension_loc, dim_x, dim_y);       //set mapsize uniform

    glDrawArrays(GL_POINTS, 0, tile_data.size());   // Draw all points from the current vao with assigned shader program
    return true;
}

bool TileObject::updateBuffers(){
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);  //focus vbo for this render object
    glBufferData(GL_ARRAY_BUFFER, tile_data.size() * sizeof(uint16_t), &tile_data[0], GL_STATIC_DRAW); //insert data from vertex_data
    return true;
}

void TileObject::update_transform(GLfloat* transform){
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
    glVertexAttribPointer(0, 1, GL_UNSIGNED_INT, GL_FALSE, 0, NULL);

    //Get uniform locations
    transform_loc = glGetUniformLocation(shader_pid, "projection");
    if(transform_loc == -1){
        printf("could not find uniform %s render_obj=%d\n", "projection", object_id);
        printDebug();
        return false;
    }

    dimension_loc = glGetUniformLocation(shader_pid, "mapSize");
    if(dimension_loc == -1){
        printf("could not find uniform %s render_obj=%d\n", "mapSize", object_id);
        printDebug();
        return false;
    }

    //check gl errors
	GLenum err;
	if ((err = glGetError()) != GL_NO_ERROR) {
		printf("buffer generation error: glenum=%d\n", err);
        printDebug();
		return false;
	}

    tile_data.push_back(1);
    updateBuffers();
    return true;
}

void TileObject::printDebug(){
    printf("debug info for TileObject %d\n", object_id);
    printf("    shader_pid: %d\n", shader_pid);
}
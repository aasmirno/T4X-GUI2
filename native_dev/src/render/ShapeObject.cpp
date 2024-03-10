#include "ShapeObject.h"

bool ShapeObject::draw(){
    glUseProgram(shader_pid);           // set shader program
    glBindVertexArray(vao_id);          // bind vertex array
    glDrawArrays(GL_TRIANGLES, 0, vertex_data.size() / 3);   // Draw the points 0 to 3 from the current vao with assigned shader program
    return true;
}

bool ShapeObject::updateBuffers(){
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);  //focus vbo for this render object
    glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), &vertex_data[0], GL_STATIC_DRAW); //insert data from vertex_data
    return true;
}

void ShapeObject::printDebug(){
    printf("Debug information for render object: %d\n", object_id);
    printf("    Type=Shape\n");
    printf("    shader_pid: %d\n", shader_pid);
    printf("    vao_id: %d\n", vao_id);
    printf("    vbo_id: %d\n", vbo_id);
    printf("    vertex_data (%d vertices)\n", vertex_data.size() / 3);
    for(int i = 0; i < vertex_data.size(); i++){
        std::cout << vertex_data[i] << " ";
    }
    std::cout << "\n";
}

bool ShapeObject::genBuffers(){
    glGenBuffers(1, &vbo_id);       // generate a vertex buffer object
    glGenVertexArrays(1, &vao_id);	// gen a vertex array object
    if (vbo_id == -1 || vao_id == -1){
        printf("buffer generation error (TriangleObject::genBuffers): vbo_id=%d vao_id=%d \n", vbo_id, vao_id);
        printDebug();
        return false;
    }

    glBindVertexArray(vao_id); //focus va buffer
    glEnableVertexAttribArray(0); //enable attrib array at 0
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);  //focus vb 

    /*
        Format for vertex buffer (what a vertex should look like):
            location: 0
            size: 3
            type: GL_FLOAT
    */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    color_loc = glGetUniformLocation(shader_pid, "color");
    if(color_loc == -1){
        printf("could not find uniform render_obj=%d\n", object_id);
        printDebug();
        return false;
    }
    setColor(shape_color);

    //check gl errors
	GLenum err;
	if ((err = glGetError()) != GL_NO_ERROR) {
		printf("buffer generation error: glenum=%d\n", err);
        printDebug();
		return false;
	}

    printf("buffers generated for render_obj=%d\n", object_id);
    return true;
}
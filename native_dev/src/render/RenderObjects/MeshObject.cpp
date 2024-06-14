#include "render/RenderObjects/MeshObject.h"

void MeshObject::printDebug()
{
    printf("DEBUG: info for MeshObject %d\n", object_id);
    printf("    shader_pid: %d\n", shader.program_id);
    printf("    proj loc: %d\n", projection_location);
    printf("    vao_id: %d\n", vao_id);
    printf("    vbo_id: %d\n", vbo_id);
}

void MeshObject::draw()
{
    glPatchParameteri(GL_PATCH_VERTICES, 4); // set patch parameter for tesselation shader

    glUseProgram(shader.program_id); // set shader program
    glBindVertexArray(vao_id);       // bind vertex array

    glDrawArrays(GL_PATCHES, 0, 4*patch_resolution*patch_resolution);
}

bool MeshObject::genBuffers()
{
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

    /*
        Initialise Shaders and get uniforms
    */
    SourcePair texture_program[2] = {
        SourcePair{"resources/mesh_shader/meshvert.glvs", GL_VERTEX_SHADER},
        SourcePair{"resources/mesh_shader/meshfrag.glfs", GL_FRAGMENT_SHADER}};
    bool shader_success = shader.createProgram(&texture_program[0], 2);
    if (!shader_success)
    {
        printDebug();
        printf("Shader Failure\n");
        return false;
    }

    // get uniform locations
    projection_location = glGetUniformLocation(shader.program_id, "projection");
    if (projection_location == -1)
    {
        printf("could not find uniform %s render_obj=%d\n", "projection", object_id);
        printDebug();
        return false;
    }

    // generate mesh vertices
    for(unsigned i = 0; i < patch_resolution - 1; i++){
        for(unsigned j = 0; j < patch_resolution - 1; j++){
            // bottom right
            vertices.push_back(-width/2.0f + width*i/(float)patch_resolution);  //x
            vertices.push_back(-heigth/2.0f + heigth*j/(float)patch_resolution);  //y
            vertices.push_back(0.0f);   //z heightmap

            vertices.push_back(-width/2.0f + width*(i+1)/(float)patch_resolution);  //x
            vertices.push_back(-heigth/2.0f + heigth*j/(float)patch_resolution);  //y
            vertices.push_back(0.0f);   //z heightmap

            vertices.push_back(-width/2.0f + width*i/(float)patch_resolution);  //x
            vertices.push_back(-heigth/2.0f + heigth*(j+1)/(float)patch_resolution);  //y
            vertices.push_back(0.0f);   //z heightmap

            vertices.push_back(-width/2.0f + width*(i+1)/(float)patch_resolution);  //x
            vertices.push_back(-heigth/2.0f + heigth*(j+1)/(float)patch_resolution);  //y
            vertices.push_back(0.0f);   //z heightmap
        }
    }

    if(!checkGLError()){
        printDebug();
        return false;
    }

    return updateBuffers();
}

bool MeshObject::setTransform(GLfloat *transform)
{
    glUseProgram(shader.program_id);
    glUniformMatrix4fv(projection_location, 1, GL_FALSE, transform);
    if (!checkGLError())
    {
        printf("ERROR: gl error in transform update\n");
        printDebug();
        return false;
    }
    return true;
}

bool MeshObject::updateBuffers()
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW); //insert data from vertex_data
    if (!checkGLError())
    {
        printf("ERROR: gl error in buffer update\n");
        printDebug();
        return false;
    }
    return true;
}
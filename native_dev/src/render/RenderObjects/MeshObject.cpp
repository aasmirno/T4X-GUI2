#include "T4X/render/RenderObjects/MeshObject.h"

void MeshObject::printDebug()
{
    printf("[ MESH DEBUG ]\n");
    printf("    obj_id: %d\n", object_id);
    printf("    shader_pid: %d\n", shader.program_id);
    printf("    proj loc: %d\n", projection_location);
    printf("    vao_id: %d\n", vao_id);
    printf("    vbo_id: %d\n", vbo_id);

    for (size_t i = 0; i < vertices.size(); i += 12)
    {
        std::cout << "(";
        for (size_t j = 0; j < 12; j++)
        {
            std::cout << vertices[i + j] << ",";
            if ((j + 1) % 3 == 0 && j != 11)
            {
                std::cout << ") | (";
            }
        }
        std::cout << ")\n";
    }

    shader.printDebug();
}

void MeshObject::draw()
{
    glPatchParameteri(GL_PATCH_VERTICES, 4); // set patch parameter for tesselation shader

    glUseProgram(shader.program_id);        // set shader program
    glBindVertexArray(vao_id);              // bind vertex array

    glDrawArrays(GL_PATCHES, 0, 4 * patch_resolution * patch_resolution);
}

bool MeshObject::genBuffers()
{
    glBindVertexArray(vao_id);             // focus va buffer
    glEnableVertexAttribArray(0);          // enable attrib array at 0
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id); // focus vb

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
    SourcePair texture_program[4] = {
        SourcePair{"meshvert.glvs", GL_VERTEX_SHADER},
        SourcePair{"meshtsc.tesc", GL_TESS_CONTROL_SHADER},
        SourcePair{"meshtse.tese", GL_TESS_EVALUATION_SHADER},
        SourcePair{"meshfrag.glfs", GL_FRAGMENT_SHADER}};
    bool shader_success = shader.createProgram(&texture_program[0], 4);
    if (!shader_success)
    {
        printDebug();
        printf("Shader Failure\n");
        return false;
    }

    // get uniform locations
    {
        auto ploc = shader.getLocation("projection");
        if (!ploc.first)
        {
            return false;
        }
        projection_location = ploc.second;

        auto mloc = shader.getLocation("model");
        if (!mloc.first)
        {
            return false;
        }
        model_location = mloc.second;

        auto vloc = shader.getLocation("view");
        if (!vloc.first)
        {
            return false;
        }
        view_location = vloc.second;
    }

    // generate mesh vertices
    for (int i = 0; i < patch_resolution - 1; i++)
    {
        for (int j = 0; j < patch_resolution - 1; j++)
        {
            vertices.push_back(-width / 2.0f + width * i / (float)patch_resolution);   // x
            vertices.push_back(-heigth / 2.0f + heigth * j / (float)patch_resolution); // y
            vertices.push_back(i / (float)(patch_resolution - 1));                     // z heightmap

            vertices.push_back(-width / 2.0f + width * (i + 1) / (float)patch_resolution); // x
            vertices.push_back(-heigth / 2.0f + heigth * j / (float)patch_resolution);     // y
            vertices.push_back(i / (float)(patch_resolution - 1));                         // z heightmap

            vertices.push_back(-width / 2.0f + width * i / (float)patch_resolution);         // x
            vertices.push_back(-heigth / 2.0f + heigth * (j + 1) / (float)patch_resolution); // y
            vertices.push_back(i / (float)(patch_resolution - 1));                           // z heightmap

            vertices.push_back(-width / 2.0f + width * (i + 1) / (float)patch_resolution);   // x
            vertices.push_back(-heigth / 2.0f + heigth * (j + 1) / (float)patch_resolution); // y
            vertices.push_back(i / (float)(patch_resolution - 1));                           // z heightmap
        }
    }

    // printDebug();

    // push to z plane
    model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    // initial transform update
    glUseProgram(shader.program_id);
    glUniformMatrix4fv(view_location, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(model_location, 1, GL_FALSE, &model[0][0]);

    if (!checkGLError())
    {
        printDebug();
        return false;
    }

    // inital buffer update
    return updateBuffers();
}

bool MeshObject::updateBuffers()
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW); // insert data from vertex_data
    if (!checkGLError())
    {
        printf("ERROR: gl error in buffer update\n");
        printDebug();
        return false;
    }
    return true;
}
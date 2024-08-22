#include "T4X/render/RenderObjects/WorldObjects/MeshObject.h"

void MeshObject::printDebug()
{
	printf("[ MESH DEBUG ]\n");
	printf("    obj_id: %d\n", object_id);
	printf("    shader_pid: %d\n", shader.program_id);
	printf("    proj loc: %d\n", projection_location);
	printf("    vao_id: %d\n", vao_id);
	printf("    vbo_id: %d\n", vbo_id);

	shader.printDebug();
}

void MeshObject::draw()
{
	glPatchParameteri(GL_PATCH_VERTICES, 4);	// set patch parameter for tesselation shader
	glUseProgram(shader.program_id);			// activate shader program
	
	// activate textures
	color_texture.activate();
	mesh_texture.activate();					
	
	// bind and draw vertices
	glBindVertexArray(vao_id);
	glDrawArrays(GL_PATCHES, 0, 4 * resolution * resolution);
}

bool MeshObject::setAttribs() {
	glBindVertexArray(vao_id);             // focus va buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id); // focus vb

	// xyz position data
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);

	// texture position data
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	if (!checkGLError()) {
		printDebug();
		return false;
	}

	return true;
}

bool MeshObject::loadShaders() {
	SourcePair mesh_program[4] = {
		SourcePair{"meshvert.glvs", GL_VERTEX_SHADER},
		SourcePair{"meshtsc.tesc", GL_TESS_CONTROL_SHADER},
		SourcePair{"meshtse.tese", GL_TESS_EVALUATION_SHADER},
		SourcePair{"meshfrag.glfs", GL_FRAGMENT_SHADER} };

	bool shader_success = shader.createProgram(&mesh_program[0], 4);
	if (!shader_success)
	{
		printDebug();
		printf("Shader Failure\n");
		return false;
	}

	// set textures
	color_texture.setTexture("MeshTex.png", GL_TEXTURE0);
	
	// activate program
	glUseProgram(shader.program_id);

	// assign texture units
	{
		auto ploc = shader.getLocation("image_texture");
		if (!ploc.first)
		{
			printf("[ MESH ERROR ] image texture uniform not found\n");
			return false;
		}
		glUniform1i(ploc.second, 0);

		ploc = shader.getLocation("data_texture");
		if (!ploc.first)
		{
			printf("[ MESH ERROR ] data texture uniform not found\n");
			return false;
		}
		glUniform1i(ploc.second, 1);

		if (!checkGLError())
		{
			printf("[ MESH ERROR ]: texture assignment error\n");
			printDebug();
			return false;
		}
	}

	// get shader locations
	{
		auto ploc = shader.getLocation("projection");
		if (!ploc.first)
		{
			printf("[ MESH ERROR ] uniform\n");
			return false;
		}
		projection_location = ploc.second;

		ploc = shader.getLocation("view");
		if (!ploc.first)
		{
			printf("[ MESH ERROR ] uniform\n");
			return false;
		}
		view_location = ploc.second;

		if (!checkGLError())
		{
			printf("[ MESH ERROR ]: uniform assignment error\n");
			printDebug();
			return false;
		}
	}

	return true;
}

bool MeshObject::updateBuffers(int size, float* data)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	if (!checkGLError())
	{
		printf("[ MESH ERROR ]: gl error in buffer update\n");
		printDebug();
		return false;
	}
	return true;
}

void MeshObject::cleanup() {
	// delete shaders
	shader.deleteProgram();
	mesh_texture.deleteTexture();
	color_texture.deleteTexture();

	//delete buffers
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDeleteBuffers(1, &vbo_id);
	glDeleteVertexArrays(1, &vao_id);

	if (!checkGLError())
	{
		printf("[ MESH ERROR ]: gl error in cleanup\n");
		printDebug();
	}
}

bool MeshObject::setMeshData(float* data, int data_width, int data_height) {
	// create mesh array
	std::vector<float> mesh_map;
	mesh_map.reserve(width * height * 3);

	// transform map parameters to math friendly values
	float mesh_width = (float)width;
	float mesh_height = (float)height;

	//generate patch map
	for (int i = 0; i <= resolution - 1; i++)
	{
		for (int j = 0; j <= resolution - 1; j++)
		{
			// bottom left patch values
			mesh_map.push_back(-(mesh_width / 2.0f) + mesh_width * (i / (float)resolution));  // x
			mesh_map.push_back(-(mesh_height / 2.0f) + mesh_height * (j / (float)resolution)); // y
			mesh_map.push_back(0.0f);
			mesh_map.push_back(i / (float)resolution); // u
			mesh_map.push_back(j / (float)resolution); // v

			// bottom right patch coordinates
			mesh_map.push_back(-(mesh_width / 2.0f) + mesh_width * ((i + 1) / (float)resolution)); // x
			mesh_map.push_back(-(mesh_height / 2.0f) + mesh_height * (j / (float)resolution));      // y
			mesh_map.push_back(0.0f);
			mesh_map.push_back((i + 1) / (float)resolution); // u
			mesh_map.push_back(j / (float)resolution); // v

			// top right patch coordinates
			mesh_map.push_back(-(mesh_width / 2.0f) + mesh_width * (i / (float)resolution));        // x
			mesh_map.push_back(-(mesh_height / 2.0f) + mesh_height * ((j + 1) / (float)resolution)); // y
			mesh_map.push_back(0.0f);
			mesh_map.push_back(i / (float)resolution); // u
			mesh_map.push_back((j + 1) / (float)resolution); // v

			// top right patch coordinates
			mesh_map.push_back(-(mesh_width / 2.0f) + mesh_width * ((i + 1) / (float)resolution));  // x
			mesh_map.push_back(-(mesh_height / 2.0f) + mesh_height * ((j + 1) / (float)resolution)); // y
			mesh_map.push_back(0.0f);
			mesh_map.push_back((i + 1) / (float)resolution); // u
			mesh_map.push_back((j + 1) / (float)resolution); // v
		}
	}

	// update vertex data
	if (!updateBuffers(mesh_map.size() * sizeof(float), &mesh_map[0])) { return false; }
	// update texture data
	mesh_texture.setTexture(data, GL_TEXTURE1, width, height); //GL rectangle texture

	if (!checkGLError())
	{
		printf("[ MESH ERROR ]: mesh data creation error\n");
		printDebug();
		return false;
	}

	return true;
}
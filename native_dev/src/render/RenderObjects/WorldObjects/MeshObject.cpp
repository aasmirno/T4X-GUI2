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
	glBindTexture(GL_TEXTURE_2D, t.handle);		// bind mesh texture
	glUseProgram(shader.program_id);			// set shader program
	glBindVertexArray(vao_id);					// bind vertex array

	glDrawArrays(GL_PATCHES, 0, 4 * resolution * resolution);
}

bool MeshObject::setAttribs() {
	glBindVertexArray(vao_id);             // focus va buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id); // focus vb

	// xyz pos data
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	if (!checkGLError()) {
		printDebug();
		return false;
	}

	t.setTexture("MeshTex.png");
	return true;
}

bool MeshObject::loadShaders() {
	// load shader
	{
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
	}
	// load uniforms
	{
		auto ploc = shader.getLocation("projection");
		if (!ploc.first)
		{
			printf("[ MESH ERROR ]\n");
			return false;
		}
		projection_location = ploc.second;

		ploc = shader.getLocation("view");
		if (!ploc.first)
		{
			printf("[ MESH ERROR ]\n");
			return false;
		}
		view_location = ploc.second;
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
	shader.deleteProgram();
}

bool MeshObject::setMeshData(float* data, unsigned size) {
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
			int x_coord = (width - 1) * (i / (float)resolution);       // map x
			int y_coord = (height - 1) * (j / (float)resolution);      // map y
			mesh_map.push_back(data[x_coord * width + y_coord]); // map elevation at the patch coordinate
			//mesh_map.push_back(0.5f);

			// bottom right patch coordinates
			mesh_map.push_back(-(mesh_width / 2.0f) + mesh_width * ((i + 1) / (float)resolution)); // x
			mesh_map.push_back(-(mesh_height / 2.0f) + mesh_height * (j / (float)resolution));      // y
			x_coord = (width - 1) * ((i + 1) / (float)resolution);      // map x
			y_coord = (height - 1) * (j / (float)resolution);           // map y
			mesh_map.push_back(data[x_coord * width + y_coord]);      // map elevation at the patch coordinate
			//mesh_map.push_back(0.5f);

			// top right patch coordinates
			mesh_map.push_back(-(mesh_width / 2.0f) + mesh_width * (i / (float)resolution));        // x
			mesh_map.push_back(-(mesh_height / 2.0f) + mesh_height * ((j + 1) / (float)resolution)); // y
			x_coord = (width - 1) * (i / (float)resolution);             // map x
			y_coord = (height - 1) * ((j + 1) / (float)resolution);      // map y
			mesh_map.push_back(data[x_coord * width + y_coord]);       // map elevation at the patch coordinate
			//mesh_map.push_back(0.5f);

			// top right patch coordinates
			mesh_map.push_back(-(mesh_width / 2.0f) + mesh_width * ((i + 1) / (float)resolution));  // x
			mesh_map.push_back(-(mesh_height / 2.0f) + mesh_height * ((j + 1) / (float)resolution)); // y
			x_coord = (width - 1) * ((i + 1) / (float)resolution);       // map x
			y_coord = (height - 1) * ((j + 1) / (float)resolution);      // map y
			mesh_map.push_back(data[x_coord * width + y_coord]);       // map elevation at the patch coordinate
			//mesh_map.push_back(0.5f);
		}
	}

	return updateBuffers(size * sizeof(float), &mesh_map[0]);
}
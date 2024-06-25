#include "T4X/render/RenderObjects/MeshObject.h"

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

	glDrawArrays(GL_PATCHES, 0, 4 * patch_resolution * patch_resolution);
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
	return true;
}

bool MeshObject::loadUniforms() {
	auto ploc = shader.getLocation("projection");
	if (!ploc.first)
	{
		return false;
	}
	projection_location = ploc.second;

	auto vloc = shader.getLocation("view");
	if (!vloc.first)
	{
		return false;
	}
	view_location = vloc.second;
	return true;
}

bool MeshObject::updateBuffers(int size, float* data)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	if (!checkGLError())
	{
		printf("ERROR: gl error in buffer update\n");
		printDebug();
		return false;
	}
	return true;
}

bool MeshObject::setMeshData(std::vector<float> data, unsigned patches) {
	patch_resolution = patches;
	return updateBuffers(data.size() * sizeof(float), &data[0]);
}
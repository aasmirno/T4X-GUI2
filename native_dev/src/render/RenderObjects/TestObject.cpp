#include "T4X/render/RenderObjects/TestObject.h"

void TestObject::printDebug() {
	printf("null\n");
}

bool TestObject::loadShaders() {
	SourcePair mesh_program[NUM_SHADERS] = {
		SourcePair{"basevert.glvs", GL_VERTEX_SHADER},
		SourcePair{"basefrag.glfs", GL_FRAGMENT_SHADER}
	};

	bool shader_success = shader.createProgram(&mesh_program[0], NUM_SHADERS);
	if (!shader_success)
	{
		printDebug();
		printf("Shader Failure\n");
		return false;
	}
	return true;
}

bool TestObject::setAttribs() {
	glBindVertexArray(vao_id);             // focus va buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id); // focus vb

	// enable vertex position input
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	updateBuffers(VERTICES * sizeof(float), &vertices[0]);
	return true;
}

bool TestObject::loadUniforms() {
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

bool TestObject::updateBuffers(int size, float* data) {
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

void TestObject::draw() {
	glUseProgram(shader.program_id);        // set shader program
	glBindVertexArray(vao_id);              // bind vertex array

	glDrawArrays(GL_TRIANGLES, 0, VERTICES / 3);
}

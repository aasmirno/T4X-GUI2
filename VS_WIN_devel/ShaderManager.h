#pragma once
#include <stdio.h>
#include <iostream>
#include <fstream>
#define GL_GLEXT_PROTOTYPES
#include <gl/glew.h>

/*
	Shader managment unit
	Handles:
		Loading, compiling, and linking shaders
		providing program id, input and uniform locations
*/
class ShaderManager {
private:
	GLuint program_id = 0;		//shader program id

	GLint vertexin_loc = -1;		//in vec2 location
	GLint transform_loc = -1;	//in float location

	GLuint gVBO = 0;
	GLuint gIBO = 0;

	/*
		Load a shader from loader
			type: GLenum of shader type
	*/
	GLuint loadFromFile(std::string path, GLenum type);

public:
	/*
		load and compile shaders
	*/
	bool init();

	//location and id getters
	GLuint getProgramID();

	GLint getVertexInLoc();
	GLint getTransformLoc();

	GLuint getVBO();
	GLuint getIBO();
};
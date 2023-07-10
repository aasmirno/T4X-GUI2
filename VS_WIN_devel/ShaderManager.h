#pragma once
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>

#define GL_GLEXT_PROTOTYPES
#include <gl/glew.h>

/*
	Shader management:
	Manage shaders loading process:
		creates program
		loads shaders
		links program
	!!MAKE SURE TO LINK INPUTS IN DERIVED CLASSES
*/
class ShaderManager {
protected:
	GLuint program_id = 0;			//shader program id

private:
	std::vector<GLint> shaders;		//shader ids
	void deleteShaders();

public:

	/*
		Load a shader from shader file
		type: GLenum of shader type
			loads shader
			compile shader
			attach shader
	*/
	bool loadFromFile(std::string path, GLenum type);

	/*
		Link the shader program
	*/
	bool linkProgram();

	//location and id getters
	GLuint getProgramID();
};
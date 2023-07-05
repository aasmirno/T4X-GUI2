#pragma once
#include <stdio.h>
#include <iostream>
#include <fstream>
#define GL_GLEXT_PROTOTYPES
#include <gl/glew.h>

class ShaderManager {
private:
	const GLfloat MIN_TRANSFORM = 0.1f;	//minimum zoom level

	GLuint gProgramID = 0;	//shader program id
	GLint gVertex2DLocation = -1;	//in vec2 location
	GLint gTransformLocation = -1;	//in float location

	GLuint gVBO = 0;
	GLuint gIBO = 0;

	/*
		Load a shader from loader
			type: GLenum of shader type
	*/
	GLuint loadFromFile(std::string path, GLenum type);

public:
	GLfloat transform = 2.0f;
	bool init();
	GLuint getID();
	GLuint getVBO();
	GLuint getIBO();
	GLint getVpos();

	void setTransform(float new_transform);
	void updateTransform();
};
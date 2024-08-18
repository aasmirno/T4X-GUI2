#pragma once
#define GL_GLEXT_PROTOTYPES
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>

/*
	OpenGL data texture object
*/
class DataTexture {
private:

	// GL parameters
	GLuint handle = 0;              // texture handle
	GLenum tex_unit = GL_TEXTURE0;	// texture unit

public:
	/*
		Create 1comp float texture
	*/
	bool createTexture(GLenum texture_unit, float* data, GLsizei data_length);

	/*
		Activate texture
	*/
	void activate();

	/*
		Free glTexture from graphics memory
	*/
	bool deleteTexture();
};
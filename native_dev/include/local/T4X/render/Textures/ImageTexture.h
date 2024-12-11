#pragma once
#define GL_GLEXT_PROTOTYPES
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <stbi/stb_image.h>

#include <iostream>

class ImageTexture
{
private:
public:
	/*
	   Texture Parameters
	*/
	// GL parameters
	GLuint handle = 0;              // texture handle
	GLenum tex_unit = GL_TEXTURE0;	// texture unit

	/*
		Print texture info
	*/
	void printInfo() {
		//std::cout << "[ TEXTURE INFO ]\n  width=" << width << "\n  height=" << height << "\n  num_channels=" << channels << "\n  tex_unit=" << tex_unit << "\n";
	}

	/*
		Check for initialisation
	*/
	bool loaded() {
		return true;
	}

	/*
		load a texture from a png file
	*/
	bool setTexture(const char* filename, GLenum texture_unit);

	/*
		load a texture with float data
	*/
	bool setTexture(float* data, GLenum texture_unit, int width, int height);

	/*
		Activate texture
	*/
	void activate();

	/*
		Free glTexture from graphics memory
	*/
	bool deleteTexture();
};
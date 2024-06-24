#pragma once
#define GL_GLEXT_PROTOTYPES
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <stbi/stb_image.h>

#include <iostream>

class RenderTexture
{
private:
public:
	/*
	   Texture Parameters
	 */
	int width = 0;                  // texture width
	int height = 0;                 // texture height
	int channels = 0;               // number of channels

	GLuint handle = 0;               // texture handle

	/*
		Print texture info
	*/
	void printInfo() {
		std::cout << "[ TEXTURE INFO ]\n  width=" << width << "\n  height=" << height << "\n  num_channels=" << channels << "\n";
	}

	/*
		Check for initialisation
	*/
	bool loaded() {
		return (handle != 0 && width != 0 && height != 0);
	}

	/*
		load a texture from a png file
	*/
	bool setTexture(const char* filename);

	/*
		Free glTexture from graphics memory
	*/
	bool deleteTexture();
};
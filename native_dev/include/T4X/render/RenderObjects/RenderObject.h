#pragma once
#include <GL/glew.h>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

#include <stdio.h>
#include <iostream>
#include <vector>

#include "T4X/render/Textures/ImageTexture.h"
#include "T4X/render/Shader.h"
#include "T4X/Utils.h"

/*
	Base class for renderable objects
*/
class RenderObject
{
protected:
	/*
		GL attributes
	*/
	GLuint vao_id = -1; // vertex array object handle
	GLuint vbo_id = -1; // vertex buffer object handle
	Shader shader;      // shader program for this object

	/*
		Update data in buffer
	*/
	virtual bool updateBuffers(int size, float* data) = 0;

	/*
		Generate shaders
	*/
	virtual bool loadShaders() = 0;

	/*
		Set buffer attributes and attribute locations
	*/
	virtual bool setAttribs() = 0;

	/*
		Check GL error
	*/
	bool checkGLError()
	{
		GLenum err;
		if ((err = glGetError()) != GL_NO_ERROR)
		{
			printf("[ RENDER OBJECT ERROR ] gl error: %d\n", err);
			return false;
		}
		return true;
	}

public:
	uint object_id = 0; // render object id

	/*
		Dump debug information to the console
	*/
	virtual void printDebug() = 0;

	/*
		Draw method for a render object
	*/
	virtual void draw() = 0;

	/*
		Free used memory
	*/
	virtual void cleanup() = 0;

	/*
		Initialise the render object
			obj_id: render object id
	*/
	bool initialise(uint id)
	{
		// set object id
		object_id = id;

		/*
			Generate buffers and set buffer parameters
		*/
		glGenBuffers(1, &vbo_id);      // generate a vertex buffer object
		glGenVertexArrays(1, &vao_id); // gen a vertex array object
		if (vbo_id == -1 || vao_id == -1)
		{
			printf("buffer generation error: vbo_id=%d vao_id=%d, obj_id=%d\n", vbo_id, vao_id, id);
			return false;
		}

		// call subclass initialisation pipeline
		if (!loadShaders()) return false;
		if (!setAttribs()) return false;
		return true;
	}
};
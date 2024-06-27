#pragma once
#include <GL/glew.h>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

#include <stdio.h>
#include <iostream>
#include <vector>

#include "T4X/render/Texture.h"
#include "T4X/render/Shader.h"
#include "T4X/Utils.h"

/*
	Base class for render objects
		object_id: id used by render manager for organisation
		shader_pid: id provided at initialisation, specifies handle of shader program to be used for drawing
		vao_id: vertex array object handle
		vbo_id: vertex_buffer object handle
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
		Set gk attributes and attribute locations
	*/
	virtual bool setAttribs() = 0;

	/*
		Load required uniforms
	*/
	virtual bool loadUniforms() = 0;

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
	int object_id = -1; // render object id

	/*
		Initialise the render object
			obj_id: render object id
	*/
	bool initialise(int obj_id)
	{
		object_id = obj_id;
		/*
			Generate buffers and set buffer parameters
		*/
		glGenBuffers(1, &vbo_id);      // generate a vertex buffer object
		glGenVertexArrays(1, &vao_id); // gen a vertex array object
		if (vbo_id == -1 || vao_id == -1)
		{
			printf("buffer generation error: vbo_id=%d vao_id=%d, obj_id=%d\n", vbo_id, vao_id, obj_id);
			return false;
		}

		// call subclass initialisation pipeline
		if (!loadShaders()) return false;
		if (!setAttribs()) return false;
		if (!loadUniforms()) return false;

		return true;
	}

	/*
		Dump debug information to the console
	*/
	virtual void printDebug() = 0;

	/*
		Draw method for a render object, implemented by sub class
	*/
	virtual void draw() = 0;
};
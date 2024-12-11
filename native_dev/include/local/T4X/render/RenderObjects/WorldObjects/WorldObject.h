#pragma once
#include <GL/glew.h>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

#include <stdio.h>
#include <iostream>

#include "T4X/render/RenderObjects/RenderObject.h"

/*
	Renderable objects affected by lighting and camera
*/
class WorldObject : public RenderObject {
protected:
	/*
		Camera uniforms
	*/
	GLint projection_location = -1; // projection matrix
	GLint view_location = -1;       // view matrix

	/*
		Lighting parameters
	*/
	GLint ambient_level_location = -1;
	GLint light_position = -1;

public:

	/*
		Set ambient light level for lighting shaders
	*/
	void setAmbient(float level) {
		if (ambient_level_location != -1) {
			glUniform1f(ambient_level_location, level);
		}
	}

	/*
		Set light source position for lighting shaders
	*/
	void setLightPosition(float x, float y, float z) {
		glm::vec3 light_pos(x,y,z);
		glUniform3fv(light_position, 1, &light_pos[0]);
	}

	/*
		Set transform matrix
			GLfloat *matrix: pointer to mat4 transform matrix
			std::string type (view/projection): specified uniform to update
	*/
	bool setTransform(GLfloat* matrix, std::string type)
	{
		// check shader init
		if (shader.program_id == 0)
		{
			printf("[ WORLD OBJECT ERROR ] uninitialised shader\n");
			return false;
		}

		// check proper type string
		if (type != "projection" && type != "view")
		{
			printf("[ WORLD OBJECT ERROR ] improper type: %s, need view or projection\n", type.c_str());
			return false;
		}

		// check locations defined and set locations
		GLint location = -1;
		if (type == "view")
		{
			if (view_location == -1) {
				printf("[ WORLD OBJECT ERROR ] no view location defined\n");
				return false;
			}
			else {
				location = view_location;
			}
		}

		if (type == "projection")
		{
			location = projection_location;
			if (projection_location == -1) {
				printf("[ WORLD OBJECT ERROR ] no projection location defined\n");
				return false;
			}
			else {
				location = projection_location;
			}
		}

		// redundant check
		if (location == -1)
		{
			printf("[ WORLD OBJECT ERROR ] unknown setTransform error, location = -1\n");
			return false;
		}

		// update transforms
		glUseProgram(shader.program_id);
		glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
		if (!checkGLError())
		{
			printf("[ WORLD OBJECT ERROR ] error in %s update\n", type.c_str());
			printDebug();
			return false;
		}
		return true;
	}
};
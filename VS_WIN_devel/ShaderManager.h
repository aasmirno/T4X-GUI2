#pragma once
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>

#include <IL/il.h>

#define GL_GLEXT_PROTOTYPES
#include <gl/glew.h>

/*
	Shader management:
	Manages generic shaders loading process:
		creates program
		loads shaders
		links program
	Manages utility methods
		load texture to a tex handle

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
		Struct for a gl texture
	*/
	struct GLTexture {
		GLuint handle = -1;			//texture handle
		std::vector<ILubyte> data;	//raw image data
		int width = -1;				//texture width
		int height = -1;			//texture height
	};

	/*
		Check gl error queue
	*/
	bool checkGLError();

	/*
		Load a shader from shader file
		type: GLenum of shader type
			loads shader
			compile shader
			attach shader
	*/
	bool loadShader(std::string path, GLenum type);

	/*
		Link a generated shader program
	*/
	bool linkProgram();

	//location and id getters
	GLuint getProgramID();

	/*
		Load a texture into a texture handle
			1
			texture_path: file path to texture
			texture_handle: texture handle
			2
			GLTexture tex: Gltexture struct of an already loaded texture
	*/
	GLTexture loadTextureFromFile(std::string texture_path);
	bool loadTexture(GLTexture tex);

	/*
	* create a Vertex Buffer Object with the given handle
	*	Gluint* vbo_id: pointer to GLuint storing vbo handle
	*/
	bool genVBO(GLuint* vbo_id);

	/*
	* create a Vertex Array Object with the given handle and assign a vertex buffer object
	*	Gluint* vao_id: pointer to GLuint storing vbo handle
	*	Gluint vbo_id: vertex buffer object handle to assign
	*/
	bool genVAO(GLuint* vao_id, GLuint vbo_id);

	/*
	* update an existing Vertex Buffer Object with new information (uint16 version)
	*	Gluint vbo_id: vertex buffer object handle to update
	*	int size: number of elements in new data
	*	uint16_t* array: pointer to data array
	*/
	bool updateVBO(GLuint vbo_id, int size, uint16_t* array);


	/*
	*	GEOMETRY SECTION
	*
	*/

	/*
	*	draw textured rectangle
	*/
	void drawTexturedRect(int rect_x, int rect_y, int rect_w, int rect_h, GLTexture tex);

};
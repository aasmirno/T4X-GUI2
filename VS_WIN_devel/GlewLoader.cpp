#include "GlewLoader.h"

GLuint GlewLoader::getID() {return gProgramID;}
GLint GlewLoader::getVpos() { return gVertexPos2DLocation;}
GLuint GlewLoader::getVBO() { return gVBO; }
GLuint GlewLoader::getIBO() { return gIBO; }

/*
	Manage glew loading process:
		load vertex shader
		load frag shader
		link program
*/
bool GlewLoader::init() {
	//load and check shaders
	gProgramID = glCreateProgram();
	if (loadVertexShader() != true) {
		printf("vertex shader issue");
		return false;
	}

	if (loadFragShader() != true) {
		printf("frag shader issue");
		return false;
	}

	//link program
	glLinkProgram(gProgramID);
	
	//check success
	GLint linkSuccess = GL_TRUE;
	glGetProgramiv(gProgramID, GL_LINK_STATUS, &linkSuccess);
	if (linkSuccess != GL_TRUE) {
		printf("Program link error: %d", gProgramID);
		return false;
	}

	gVertexPos2DLocation = glGetAttribLocation(gProgramID, "LVertexPos2D");
	if (gVertexPos2DLocation == -1) {
		printf("LVertexPos2D is not valid in glsl\n");
		return false;
	}

	/*
		Temporarily hardcoded buffer gen
	*/

	//VBO data
	GLfloat vertexData[] =
	{
		 0.1f, -0.1f,
		 0.5f,  0.1f,
		-0.1f,  0.1f
	};

	//IBO data
	GLuint indexData[] = { 0, 1, 2 };

	//Create VBO
	glGenBuffers(1, &gVBO);
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

	//Create IBO
	glGenBuffers(1, &gIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indexData, GL_STATIC_DRAW);

	return true;
}

/*
	attempt to attach a vertex shader, defined in method
	returns true on success
*/
bool GlewLoader::loadVertexShader() {
	const GLchar* vertexShaderSource[] =
	{
		"#version 130\nin vec2 LVertexPos2D; void main() { gl_Position = vec4( LVertexPos2D.x, LVertexPos2D.y, 0, 1 ); }"
	};

	//load and compile shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	//check shader
	GLint vShaderCompiled = GL_FALSE;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vShaderCompiled);
	if (vShaderCompiled != GL_TRUE) {
		printf("vert shader compilation error: %d\n", vertexShader);
		return false;
	}

	//attach shader
	glAttachShader(gProgramID, vertexShader);
	return true;
}

/*
	attempt to attach a frag shader, defined in method 
	returns true on success
*/
bool GlewLoader::loadFragShader() {
	const GLchar* fragShaderSource[] = {
		"#version 130\nout vec4 LFragment; void main() { LFragment = vec4( 1.0, 1.0, 1.0, 1.0 ); }"
	};

	//load and compile shader
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, fragShaderSource, NULL);
	glCompileShader(fragShader);

	//check shader
	GLint fShaderCompiled = GL_FALSE;
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &fShaderCompiled);
	if (fShaderCompiled != GL_TRUE) {
		printf("frag shader compilation error: %d\n", fragShader);
		return false;
	}

	//attach shader
	glAttachShader(gProgramID, fragShader);
	return true;
}
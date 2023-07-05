#include "ShaderManager.h"

GLuint ShaderManager::getID() { return gProgramID; }
GLint ShaderManager::getVpos() { return gVertex2DLocation; }
GLuint ShaderManager::getVBO() { return gVBO; }
GLuint ShaderManager::getIBO() { return gIBO; }

void ShaderManager::updateTransform() {
	setTransform(transform);
}

void ShaderManager::setTransform(float new_transform) {
	//if the zoom level is less than the floor
	if (new_transform < MIN_TRANSFORM) {
		transform = MIN_TRANSFORM;
	}
	else {
		transform = new_transform;
	}

	//set transform
	glUseProgram(gProgramID);
	glUniform1f(gTransformLocation, transform);
}

/*
	Manage shaders loading process:
		load vertex shader
		load frag shader
		link program
		link uniforms/attribs
*/
bool ShaderManager::init() {
	//create shader pipeline
	gProgramID = glCreateProgram();


	//load and check shaders
	GLuint vertexShader = loadFromFile("D:\\Software and Tools\\C++\\T4x\\VS_WIN_devel\\x64\\Debug\\vert.glvs", GL_VERTEX_SHADER);
	if (vertexShader == 0) {
		glDeleteProgram(gProgramID);
		gProgramID = 0;
		return false;
	}
	glAttachShader(gProgramID, vertexShader);

	GLuint fragShader = loadFromFile("D:\\Software and Tools\\C++\\T4x\\VS_WIN_devel\\x64\\Debug\\frag.glfs", GL_FRAGMENT_SHADER);
	if (fragShader == 0) {
		glDeleteProgram(gProgramID);
		gProgramID = 0;
		return false;
	}
	glAttachShader(gProgramID, fragShader);


	//link and check program
	glLinkProgram(gProgramID);
	GLint linkSuccess = GL_FALSE;
	glGetProgramiv(gProgramID, GL_LINK_STATUS, &linkSuccess);
	if (linkSuccess != GL_TRUE) {
		printf("Program link error: %d", gProgramID);
		glDeleteShader(vertexShader);
		glDeleteShader(fragShader);
		glDeleteProgram(gProgramID);
		return false;
	}

	//clean up shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);

	//assign shader inputs and uniforms and check
	gVertex2DLocation = 0;
	glBindAttribLocation(gProgramID, 0, "Vertex2D");	//in vec2

	gTransformLocation = glGetUniformLocation(gProgramID, "transform"); //in transform

	//init transform
	glUseProgram(gProgramID);
	glUniform1f(gTransformLocation, transform);

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

GLuint ShaderManager::loadFromFile(std::string path, GLenum type) {
	GLuint id = 0;

	std::string shader_raw;
	std::ifstream file_source(path.c_str());

	if (file_source) {
		//read file
		shader_raw.assign((std::istreambuf_iterator< char >(file_source)), std::istreambuf_iterator< char >());

		//create shader and set source
		id = glCreateShader(type);

		const GLchar* shader_source = shader_raw.c_str();
		glShaderSource(id, 1, (const GLchar**)&shader_source, NULL);

		//compile
		glCompileShader(id);

		//error checking
		GLint compiled = GL_FALSE;
		glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
		if (compiled != GL_TRUE) {
			printf("shader comp error: %d in\n %s\n", id, shader_source);

			//cleanup on comp failure
			glDeleteShader(id);
			id = 0;
		}
	}
	else {
		printf("bad file: %s\n", path.c_str());
	}

	return id;
}
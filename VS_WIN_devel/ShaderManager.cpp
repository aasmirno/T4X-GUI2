#include "ShaderManager.h"

GLuint ShaderManager::getProgramID() { return program_id; }

bool ShaderManager::linkProgram() {
	//link and check program
	glLinkProgram(program_id);
	GLint linkSuccess = GL_FALSE;
	glGetProgramiv(program_id, GL_LINK_STATUS, &linkSuccess);
	if (linkSuccess != GL_TRUE) {
		printf("Program link error: %d", program_id);
		deleteShaders();
		glDeleteProgram(program_id);
		return false;
	}

	//clean up shaders
	deleteShaders();
	return true;
}

void ShaderManager::deleteShaders() {
	for (size_t i = 0; i < shaders.size(); i++) {
		glDeleteShader(shaders.at(i));
	}
}

bool ShaderManager::loadFromFile(std::string path, GLenum type) {
	assert(program_id != 0);	//make sure program is loaded

 	GLuint shader_id = 0;

	std::string shader_raw;
	std::ifstream file_source(path.c_str());

	if (file_source) {
		//read file
		shader_raw.assign((std::istreambuf_iterator< char >(file_source)), std::istreambuf_iterator< char >());

		//create shader and set source
		shader_id = glCreateShader(type);

		const GLchar* shader_source = shader_raw.c_str();
		glShaderSource(shader_id, 1, (const GLchar**)&shader_source, NULL);

		//compile
		glCompileShader(shader_id);

		//error checking
		GLint compiled = GL_FALSE;
		glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled);
		if (compiled != GL_TRUE) {
			printf("shader comp error: %d in\n %s\n", shader_id, shader_source);
			

			//cleanup on comp failure
			glDeleteShader(shader_id);
			shader_id = 0;
			return false;
		}
	}
	else {
		printf("bad file: %s\n", path.c_str());
		return false;
	}

	//check shader created
	if (shader_id == 0) {
		printf("shader creation error: deleting pid");
		glDeleteProgram(program_id);
		program_id = 0;
		return false;
	}

	//attach shader to program
	glAttachShader(program_id, shader_id);
	shaders.push_back(shader_id);
	return true;
}
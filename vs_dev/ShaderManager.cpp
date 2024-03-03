#include "ShaderManager.h"

//utils
GLuint ShaderManager::getProgramID() { return program_id; }

bool ShaderManager::checkGLError() {
	//check gl errors
	GLenum err;
	if ((err = glGetError()) != GL_NO_ERROR) {
		logError("gl error (ShaderManager)");
		return false;
	}
	return true;
}

//shader/program
bool ShaderManager::linkProgram(int pid) {
	//link and check program
	glLinkProgram(pid);
	GLint linkSuccess = GL_FALSE;
	glGetProgramiv(pid, GL_LINK_STATUS, &linkSuccess);
	if (linkSuccess != GL_TRUE) {
		printf("Program link error: %d", pid);
		deleteShaders();
		glDeleteProgram(pid);
		return false;
	}

	//clean up shaders
	deleteShaders();
	checkGLError();
	return true;
}

void ShaderManager::deleteShaders() {
	for (size_t i = 0; i < shaders.size(); i++) {
		glDeleteShader(shaders.at(i));
	}
	checkGLError();
}

bool ShaderManager::loadShader(std::string path, GLenum type, int pid) {
	assert(pid != 0);	//make sure program is loaded

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
		glDeleteProgram(pid);
		pid = 0;
		return false;
	}

	//attach shader to program
	glAttachShader(pid, shader_id);
	shaders.push_back(shader_id);
	checkGLError();
	return true;
}

//textures
ShaderManager::GLTexture ShaderManager::loadTextureFromFile(std::string texture_path) {
	//create an openil image
	ILuint img = -1;
	ilInit();
	ilGenImages(1, &img);	//gen img buffer
	ilBindImage(img);		//bind created image

	//load the image file
	ilLoadImage(&texture_path[0]);
	if ((ilGetInteger(IL_IMAGE_CHANNELS) != 4)) {
		printf("error: %d", ilGetInteger(IL_IMAGE_CHANNELS));
	}
	assert(ilGetInteger(IL_IMAGE_CHANNELS) == 4);	//assert rgba format
	ILubyte* bytes = ilGetData();					//get pixel data

	//generate a texture handle
	GLuint texture_handle = -1;
	glGenTextures(1, &texture_handle);

	//check gl errors
	checkGLError();

	GLTexture tex;
	tex.width = ilGetInteger(IL_IMAGE_WIDTH);
	tex.height = ilGetInteger(IL_IMAGE_HEIGHT);
	tex.handle = texture_handle;
	//resize buffer and copy pixels;
	tex.data.resize(tex.height * tex.width * 4);
	ilCopyPixels(0, 0, 0, tex.width, tex.height, 1, IL_RGBA, IL_UNSIGNED_BYTE, &tex.data[0]);

	//delete openil image
	ilDeleteImages(1, &img);
	return tex;
}

bool ShaderManager::loadTexture(GLTexture tex) {
	//error checking
	if (tex.handle == -1 || tex.height == -1 || tex.width == -1 || tex.data.size() == 0) {
		printf("texture load error (ShaderManager::loadTexture): handle=%d height=%d width=%d data_buffer=%d", tex.handle, tex.height, tex.width, (int)tex.data.size());
		return false;
	}

	//load the texture into the opengl sampler and format
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &tex.data[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	return true;
}

//vbo/vao utils
bool ShaderManager::genVBO(GLuint* vbo_id) {
	glGenBuffers(1, vbo_id);	//generate a buffer
	if (*vbo_id == -1) {
		printf("VBO gen error (ShaderManager::genVBO): id (%d)\n", *vbo_id);
		return false;
	}
	checkGLError();
	return true;
}

bool ShaderManager::genVAO(GLuint* vao_id, GLuint vbo_id) {
	//vertex array object creation
	*vao_id = -1;					//reset vertex array handle
	glGenVertexArrays(1, vao_id);	//gen a vertex array object

	//error checking
	if (*vao_id == -1 || vbo_id == -1) {
		printf("vertex array creation error (ShaderManager::genVBO): vao_id=%d, vbo_id=%d\n", *vao_id, vbo_id);
		return false;
	}

	//bind both arrays to the global context
	glBindVertexArray(*vao_id);				//bind vertex array object 
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);	//load the vbo to global context

	glEnableVertexAttribArray(0);	//enable tile id attribute in vertex shader 
	glVertexAttribIPointer(0, 1, GL_UNSIGNED_SHORT, 0, 0);	//assign vbo to vao
	//attrib index 0
	//1 component per attribute
	//gl ushort(16) array
	//uint16_t stride (16 bytes)
	//0 offset
	checkGLError();
	return true;
}

bool ShaderManager::updateVBO(GLuint vbo_id, int size, uint16_t* array) {
	if (vbo_id == -1) {
		printf("vbo uninitialised (ShaderManager::updateVBO)\n");
		return false;
	}
	if (size < -1) {
		printf("incorrect size (ShaderManager::updateVBO): %d\n", size);
		return false;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);	//bind the buffer to the global array buffer
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(uint16_t), array, GL_STATIC_DRAW);	//copy tile ids to the buffer
	return true;
}
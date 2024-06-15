#include "render/Shader.h"

GLuint Shader::loadShader(std::string path, GLenum type)
{
	GLuint shader_id = 0;

	std::string shader_raw;
	std::ifstream file_source(path.c_str());

	if (file_source)
	{
		// read file
		shader_raw.assign((std::istreambuf_iterator<char>(file_source)), std::istreambuf_iterator<char>());

		// create shader and set source
		shader_id = glCreateShader(type);

		const GLchar *shader_source = shader_raw.c_str();
		glShaderSource(shader_id, 1, (const GLchar **)&shader_source, NULL);

		// compile
		glCompileShader(shader_id);

		// error checking
		GLint compiled = GL_FALSE;
		glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled);
		if (compiled != GL_TRUE)
		{
			printf("    shader compile error:\n		%s\n", path.c_str());

			// cleanup on comp failure
			glDeleteShader(shader_id);
			return 0;
		}
	}
	else
	{
		printf("    shader creation error: \n   bad file: %s\n", path.c_str());
		return 0;
	}

	// check shader created
	if (shader_id == 0)
	{
		printf("    shader creation error\n");
		return 0;
	}

	printf("    success loading shader=%d from %s\n", shader_id, path.c_str());
	return shader_id;
}

bool Shader::createProgram(SourcePair *pair, uint num_shaders)
{
	program_id = glCreateProgram();
	if(program_id == 0){
		printf("ERROR: gl program create error\n");
		return false;
	}

	bool shader_create_failure = false;
	for (uint i = 0; i < num_shaders; i++)
	{
		GLuint shader = loadShader(pair->source, pair->type);
		if(shader == 0){ shader_create_failure = true; } else {shader_handles.push_back(shader); };
		pair++;
	}

	if(shader_create_failure == true){
		printf("ERROR: shader creation failure\n");
		deleteProgram();
		return false;
	}

	for(int i = 0; i < shader_handles.size(); i++){
		glAttachShader(program_id, shader_handles[i]);
	}
	glLinkProgram(program_id);

	return true;
}

bool Shader::deleteProgram(){
	if (program_id == 0){
		printf("ERROR: attempted to delete shader program with id 0\n");
		return false;
	}
	
	//delete all shaders in program
	for(int i = 0; i < shader_handles.size(); i++){
		glDeleteShader(shader_handles[i]);
	}
	shader_handles.clear();

	//delete program
	glDeleteProgram(program_id);
	program_id = 0;
	return true;
}

void Shader::printDebug(){
	printf("[ SHADER DEBUG ]\n");
	if(program_id == 0){
		printf("	prog_id = 0, shader failed or uninitialised\n");
		return;
	}

	printf("	program_id=%d\n", program_id);
	GLint debug = 0;
	glGetProgramiv(program_id, GL_ATTACHED_SHADERS, &debug);
	printf("	num shaders =%d\n", debug);
}
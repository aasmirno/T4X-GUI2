#include "ShaderManager.h"

GLuint ShaderManager::loadShader(std::string path, GLenum type)
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
			printf("    shader %d compile error:\n%s\n", shader_id, shader_source);

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

ShaderProgram ShaderManager::createProgram(SourcePair *pair, uint num_shaders)
{
	ShaderProgram shaders;	//struct for program and shader handles	
	GLuint program = glCreateProgram();
	shaders.program_id = program;

	bool shader_create_failure = false;
	printf("creating new shader program\n");
	for (uint i = 0; i < num_shaders; i++)
	{
		GLuint shader = loadShader(pair->source, pair->type);
		if(shader == 0){ shader_create_failure = true; } else {shaders.shader_handles.push_back(shader); };
		printf("    shader %d: %s : %d\n", i, pair->source, shader_create_failure);
		pair++;
	}

	if(shader_create_failure == true){
		printf("shader creation failed\n");
		deleteProgram(shaders);
		return shaders;
	}

	for(int i = 0; i < shaders.shader_handles.size(); i++){
		glAttachShader(program, shaders.shader_handles[i]);
	}
	glLinkProgram(program);

	printf("created shader program: %d\n", program);
	return shaders;
}

bool ShaderManager::deleteProgram(ShaderProgram program){
	if (program.program_id == 0){
		printf("delete program error, program id=0");
		return false;
	}
	
	//delete all shaders in program
	for(int i = 0; i < program.shader_handles.size(); i++){
		glDeleteShader(program.shader_handles[i]);
	}
	program.shader_handles.clear();

	//delete program
	glDeleteProgram(program.program_id);
	program.program_id = 0;
	return true;
}

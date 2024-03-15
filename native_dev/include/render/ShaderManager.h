#pragma once
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <GL/glew.h>

struct SourcePair
{
    char source[1024]; // source file path
    GLenum type;       // source shader type
};

struct ShaderProgram
{
    // program id
    GLuint program_id = 0;
    // shader handles
    std::vector<GLuint> shader_handles;

    bool clear(){
        for(int i = 0; i < shader_handles.size(); i++){
            glDeleteShader(shader_handles[i]);
        }
        
        glDeleteProgram(program_id);
        program_id = 0;
        shader_handles.clear();
        return true;
    }
};

class ShaderManager
{
private:
    /* 
        loadShader:
        load a shader of a certain type from a source file
        returns 0 on shader fail else returns shader handle
    */
    GLuint loadShader(std::string path, GLenum type);

public:
    /*
        createProgram:
        Creates a shader program given an array of source pairs consisting of {file path to source, shader type}
            SourePair *program: pointer to first element in shader program array
            uint num_shaders: number of shaders in the program
            returns a struct with program and seperate shader handles
    */
    ShaderProgram createProgram(SourcePair *program, uint num_shaders);

    bool deleteProgram(ShaderProgram program);
};
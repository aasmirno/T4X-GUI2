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

class Shader
{
private:
    // shader handles
    std::vector<GLuint> shader_handles;

    /* 
        loadShader:
        load a shader of a certain type from a source file
        returns 0 on shader fail else returns shader handle
    */
    GLuint loadShader(std::string path, GLenum type);
public:
    GLuint program_id = 0;  //opengl shader handle

    /*
        createProgram:
        Creates a shader program given an array of source pairs consisting of {file path to source, shader type}
            SourePair *program: pointer to first element in shader program array
            uint num_shaders: number of shaders in the program
            returns a struct with program and seperate shader handles
    */
    bool createProgram(SourcePair *program, uint num_shaders);

    bool deleteProgram();
};
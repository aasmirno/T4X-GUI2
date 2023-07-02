#pragma once
#include <stdio.h>
#include <iostream>

#include <gl/glew.h>

class GlewLoader {
private:
	GLuint gProgramID = 0;
	GLint gVertexPos2DLocation = -1;
	GLuint gVBO = 0;
	GLuint gIBO = 0;
public:
	bool init();
	GLuint getID();
	GLuint getVBO();
	GLuint getIBO();
	GLint getVpos();
private:
	bool loadVertexShader();
	bool loadFragShader();
};
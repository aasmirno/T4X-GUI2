#pragma once
#include <gl/glew.h>

class VBOWrapper {
private:
	GLuint gVBO = 0;	//vertex buffer id
	GLuint gIBO = 0;	//index buffer id
public:
	VBOWrapper();
	bool Draw();
};
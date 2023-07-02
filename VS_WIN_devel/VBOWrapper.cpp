#include "VBOWrapper.h"

VBOWrapper::VBOWrapper() {
	glGenBuffers(1, &gVBO);
}

bool VBOWrapper::Draw() {
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	return false;
}

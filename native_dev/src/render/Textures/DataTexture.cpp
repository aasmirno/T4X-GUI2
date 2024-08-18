#include "T4X/render/Textures/DataTexture.h"

bool DataTexture::createTexture(GLenum texture_unit, float* data, GLsizei data_length)
{
	// reset texture data
	handle = 0;
	tex_unit = texture_unit;

	// generate opengl texture and set handle
	glGenTextures(1, &handle);
	if (handle == 0)
	{
		printf("[ DATA TEXTURE ERROR ]: gl texture gen error\n");
		return false;
	}

	// set texture
	glActiveTexture(tex_unit);
	glBindTexture(GL_TEXTURE_1D, handle);

	// 1 component float texture
	glTexImage1D(handle, 0, GL_RED, data_length, 0, GL_RED, GL_FLOAT, data);

};

void DataTexture::activate() {
	glActiveTexture(tex_unit);
	glBindTexture(GL_TEXTURE_1D, handle);
}

bool DataTexture::deleteTexture()
{
	glDeleteTextures(1, &handle);
	handle = 0;
	return true;
}
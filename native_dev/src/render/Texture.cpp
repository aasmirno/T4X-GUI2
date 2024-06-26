#include "T4X/render/Texture.h"

bool RenderTexture::setTexture(const char* filename)
{
	// reset texture data
	handle = 0;
	width = 0;
	height = 0;

	// load image data
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);
	if (data == nullptr) {
		printf("[ TEXTURE ERROR ]: stbi error: %s\n", stbi_failure_reason());
		return false;
	}

	// generate opengl texture and set handle
	glGenTextures(1, &handle);
	if (handle == 0)
	{
		printf("[ TEXTURE ERROR ]: gl texture gen error\n");
		return false;
	}

	// send data to opengl
	glBindTexture(GL_TEXTURE_2D, handle);
	switch (channels) {
	case 4: // RGBA source
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
		break;
	case 3: // RGB source
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, &data[0]);
		break;
	default:
		break;
	}

	// auto generate mipmap
	glGenerateMipmap(GL_TEXTURE_2D);

	// Set parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);					// repeat s dim
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);					// reapeat t dim
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);				// min filter mipmap sampling
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);				// mag filter linear sampling

	// free image data
	stbi_image_free(data);
	return true;
}

bool RenderTexture::deleteTexture()
{
	glDeleteTextures(1, &handle);
	handle = 0;
	return true;
}

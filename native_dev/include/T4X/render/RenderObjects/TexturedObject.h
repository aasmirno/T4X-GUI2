#include "T4X/render/RenderObjects/RenderObject.h"

/*
	Renderable object representing a textured rectangle
*/
class TexturedObject : public RenderObject
{

	GLint dimension_location = -1;

	// base rectangle parameters
	GLfloat origin[2] = { 0.0f, 0.0f };				// origin for this object
	GLfloat dimensions[2] = { 100.0f, 100.0f };		//width, height

	ImageTexture texture; // texture for this object
private:
	/*
		Object parameters
	*/
	const uint NUM_SHADERS = 3;             // number of shaders
	glm::mat4 model = glm::mat4(1.0f);


	/*
		Parent class init pipeline methods
	*/
	virtual bool setAttribs();
	virtual bool loadShaders();

	virtual bool updateBuffers(int size, float* data);

public:
	/*
		Set a texture for this object
	*/
	bool loadTexture(const char* filename);
	bool setDimensions(int height, int width);
	bool setOrigin(float x, float y);

	/*
		Print debug info for this object
	*/
	virtual void printDebug();

	/*
		Draw this object
	*/
	virtual void draw();

	/*
		Destroy any memory using object belonging to this instance
	*/
	virtual void cleanup()
	{
		glDisableVertexAttribArray(0);

		shader.deleteProgram();  // delete shaders
		texture.deleteTexture(); // delete texture

		//delete buffers
		glDeleteBuffers(1, &vbo_id);
		glDeleteVertexArrays(1, &vao_id);

		if (!checkGLError())
		{
			printf("[ MESH ERROR ]: gl error in cleanup\n");
			printDebug();
		}
	}
};
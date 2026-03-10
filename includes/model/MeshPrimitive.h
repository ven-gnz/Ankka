#include <glad/glad.h>


struct MeshPrimitive

{
	GLuint vao;
	GLuint drawMode;

	bool indexed;
	GLenum indexType;
	int indexCount;
	int vertexCount;

	bool skinned;

};
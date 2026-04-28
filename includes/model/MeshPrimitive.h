#include <glad/glad.h>
#include <vector>


struct MeshPrimitive

{
	GLuint vao;
	GLuint drawMode;
	std::vector<GLuint> vbos;
	GLuint ebo;

	bool indexed;
	GLenum indexType;
	int indexCount;
	int vertexCount;

	bool skinned;
	GLuint texture = 0;
};
#pragma once
#include <vector>
#include <array>


class Mesh
{
public:
	virtual void render() const = 0;
};

struct GltfPrimitive
{
	GLuint vao = 0;
	std::array<GLuint, 5> vbos{};
	std::array<int, 5> accessors{};

	GLuint ebo = 0;
	uint32_t indexCount = 0;

	GLenum indexType = GL_UNSIGNED_SHORT;
	int material = -1;
};

struct GltfMesh : public Mesh
{
	std::vector<GltfPrimitive> primitives;
	void render() const override
	{

	}
};
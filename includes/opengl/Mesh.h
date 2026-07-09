#pragma once
#include <vector>


class Mesh
{
public:
	virtual void render() const = 0;
};

struct GltfPrimitive
{
	GLuint vao = 0;
std:array<GLuint, AttributeCount> vbos{};
	std::array<int, ATTRIBUTE_COUNT> accessors{};

	GLuint ebo = 0;
	uint32_t indexCount = 0;

	GLenum indexType = GL_UNSIGNED_SHORT;
	int material = -1;
};

struct GltfMesh : public Mesh
{
	std::vector<GltfPrimitive> primitives;
	void render() const override;
};
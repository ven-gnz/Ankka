#pragma once
#include <vector>
#include <array>
#include <tools/Logger.h>
#include <opengl/Texture.h>


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
	uint32_t vertexCount = 0;
	GLuint tex;

	GLenum indexType = GL_UNSIGNED_SHORT;
	int material = -1;

	void render()
	{
		glBindVertexArray(vao);
		glBindTexture(GL_TEXTURE_2D, tex);
		if (ebo != 0)
		{
			glDrawElements(
				GL_TRIANGLES,
				indexCount,
				indexType,
				nullptr
			);
		}
		else
		{
			glDrawArrays(GL_TRIANGLES, 0, vertexCount);
		}
	}
};

struct GltfMesh : public Mesh
{
	std::vector<GltfPrimitive> primitives;
	void render() const override
	{
		for (GltfPrimitive p : primitives)
		{
			p.render();
		}
	}
};
#pragma once
#include <vector>
#include <array>
#include <tools/Logger.h>
#include <opengl/Texture.h>
#include <assert.h>


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
	// Cpu copies for singletime bindpose
	std::vector<glm::u16vec4> joints{}; 
	std::vector<glm::vec4> weights;
	std::vector<glm::vec3> positions; 
	std::vector<glm::vec3> normals;
	GLuint ebo = 0;
	uint32_t indexCount = 0;
	uint32_t vertexCount = 0;
	GLuint tex = 0;

	GLenum indexType = GL_UNSIGNED_SHORT;
	int material = -1;

	void render() const
	{
		glBindVertexArray(vao);
		GLint v;
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &v);
		assert(v != 0);
		
		glBindTexture(GL_TEXTURE_2D, tex);
		if (ebo != 0)
		{
			assert(indexCount != 0);
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
		for (const auto& p : primitives)
		{
			p.render();
		}
	}
};
#version 460 core

layout (std140, binding = 0) uniform Matrices
{
	mat4 view;
	mat4 projection;
};

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

//layout (location = 0) out vec4 texColor;
//layout (location = 1) out vec2 texCoord;

uniform mat4 model;


void main() 
{

	gl_Position = projection * view * model *vec4(aPos, 1.0);
	//texColor = vec4(aColor, 1.0);
	//texCoord = aTexCoord;
}
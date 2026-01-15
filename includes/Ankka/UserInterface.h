#pragma once
#include "opengl/OGLRenderData.h"

class UserInterface
{
public:
	void init(OGLRenderData& renderData);
	void createFrame(OGLRenderData& renderData);
	void render();
	void cleanup();
};
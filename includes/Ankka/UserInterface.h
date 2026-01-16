#pragma once
#include "opengl/OGLRenderData.h"

class UserInterface
{

private:
	float framesPerSecond = 0.0f;
	float averagingAlpha = 0.96f;

	bool checkBoxChecked = false;

public:
	void init(OGLRenderData& renderData);
	void createFrame(OGLRenderData& renderData);
	void render();
	void cleanup();
};
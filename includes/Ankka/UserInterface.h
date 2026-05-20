#pragma once
#include "opengl/OGLRenderData.h"
#include <vector>

class UserInterface
{

private:
	std::vector<float> mFPSValues{};
	int mNumFPSValues = 90;
	float mFramesPerSecond = 0.0f;
	float averagingAlpha = 0.96f;

	bool checkBoxChecked = false;

public:
	void init(OGLRenderData& renderData);
	void createFrame(OGLRenderData& renderData);
	void render();
	void cleanup();
};
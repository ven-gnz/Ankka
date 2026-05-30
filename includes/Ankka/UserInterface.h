#pragma once
#include "opengl/OGLRenderData.h"
#include <vector>

class UserInterface {
public:
    void init(OGLRenderData& renderData);
    void createFrame(OGLRenderData& renderData);
    void render();
    void cleanup();

private:
	std::vector<float> mFPSValues{};
	int mNumFPSValues = 90;
	float mFramesPerSecond = 0.0f;
	float averagingAlpha = 0.96f;

    std::vector<float> mFPSValues{};
    int mNumFPSValues = 90;

    std::vector<float> mFrameTimeValues{};
    int mNumFrameTimeValues = 90;

    std::vector<float> mModelUploadValues{};
    int mNumModelUploadValues = 90;

    std::vector<float> mMatrixGenerationValues{};
    int mNumMatrixGenerationValues = 90;

    std::vector<float> mIKValues{};
    int mNumIKValues = 90;

    std::vector<float> mMatrixUploadValues{};
    int mNumMatrixUploadValues = 90;

    std::vector<float> mUiGenValues{};
    int mNumUiGenValues = 90;

    std::vector<float> mUiDrawValues{};
    int mNumUiDrawValues = 90;
};
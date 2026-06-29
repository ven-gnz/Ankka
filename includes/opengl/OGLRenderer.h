#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "opengl/Framebuffer.h"
#include "opengl/VertexBuffer.h"
#include "opengl/Texture.h"
#include "opengl/Shader.h"
#include "opengl/UniformBuffer.h"
#include "opengl/ShaderStorageBuffer.h"
#include "opengl/CoordArrowsModel.h"
#include "opengl/TextureBuffer.h"

#include <model/GltfInstance.h>
#include <model/GltfModel.h>


#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

#include <tools/UserInterface.h>

#include <tools/Timer.h>
#include <tools/Camera.h>
#include <tools/Raycaster.h>

#include "OGLRenderData.h"

class OGLRenderer
{
public:
	OGLRenderer(GLFWwindow* win);
	bool init(unsigned int width, unsigned int height);
	void setSize(unsigned int width, unsigned int height);
	void cleanup();
	void uploadData(OGLMesh vertexData);
	void draw();
	void handleKeyEvents(int key, int scancode, int action, int mods);

	void handleMouseButtonEvents(int button, int action, int mods);
	void handleMousePositionEvents(double xPos, double yPos);

	void toggleVsync();
	void setModelMatrix(const glm::mat4& model);

private:

	Camera mCamera{};

	bool mMouseLock = false;
	int mMouseXPos = 0;
	int mMouseYPos = 0;

	double lastTickTime = 0.0;

	void handleMovementKeys();

	Shader mGltfGPUShader{};
	Shader mChangedShader{};
	Framebuffer mFramebuffer{};
	VertexBuffer mVertexBuffer{};
	UniformBuffer mUniformBuffer{};
	
	ShaderStorageBuffer mGltfShaderStorageBuffer{};
	TextureBuffer mGltfTextureBuffer{};

	// deprecated?
	ShaderStorageBuffer mShaderStorageBuffer1{};
	ShaderStorageBuffer mShaderStorageBuffer2{};

	ShaderStorageBuffer mGltfDualQuatSSBuffer{};
	ShaderStorageBuffer mGltfDualQuatSSBuffer1{};
	ShaderStorageBuffer mGltfDualQuatSSBuffer2{};

	Texture mTex{};
	bool old_VSync = true;

	bool isMove = false;
	bool mModelUploadRequired = true;

	OGLRenderData mRenderData{};
	Raycaster rc{};

	void reorient_camera();

	glm::mat4 mViewMatrix;
	glm::mat4 mProjectionMatrix;

	UserInterface mUserInterface{};

	Timer mFrameTimer{};
	Timer mMatrixGenerateTimer{};
	Timer mIKTimer{};
	Timer mUploadToVBOTimer{};
	Timer mUploadToUBOTimer{};
	Timer mUIGenerateTimer{};
	Timer mUIDrawTimer{};

	//Debug drawing stuffs
	Shader mLineShader{};
	CoordArrowsModel mCoordArrowsModel{};
	OGLMesh mCoordArrowsMesh{};
	std::shared_ptr<OGLMesh> mLineMesh = nullptr;

	std::vector<std::shared_ptr<GltfModel>> mGltfModels{};
	std::vector<std::shared_ptr<GltfInstance>> mGltfInstances{};
	std::vector<std::shared_ptr<GltfInstance>> mGltfMatrixInstances{};
	std::vector<std::shared_ptr<GltfInstance>> mGltfDQInstances{};

	std::vector<glm::mat4> mModelJointMatrices{};
	std::vector<glm::mat2x4>mModelJointDualQuats{};

	unsigned int mSkeletonLineIndexCount = 0;
	unsigned int mCoordArrowsLineIndexCount = 0;

	Shader mGltfGPUDualQuatShader{};
	std::shared_ptr<GltfModel> mGltfModel = nullptr;
	std::shared_ptr<GltfModel> mGltfModel1 = nullptr;
	std::shared_ptr<GltfModel> mGltfModel2 = nullptr;
	std::vector<glm::mat4> renderMatrices{};

	double mLastTickTime = 0.0;

};
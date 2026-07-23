#include "opengl/OGLRenderer.h"
#include "tools/Logger.h"
#include <iostream>
#include <filesystem>
#include <string>
#include <ostream>

void OGLRenderer::handleMovementKeys()
{
	mRenderData.rdMoveForward = 0;
	if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_W) == GLFW_PRESS) {
		mRenderData.rdMoveForward += 1;
	}
	if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_S) == GLFW_PRESS) {
		mRenderData.rdMoveForward -= 1;
	}

	mRenderData.rdMoveRight = 0;
	if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_A) == GLFW_PRESS) {
		mRenderData.rdMoveRight -= 1;
	}
	if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_D) == GLFW_PRESS) {
		mRenderData.rdMoveRight += 1;
	}

	mRenderData.rdMoveUp = 0;
	if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_E) == GLFW_PRESS) {
		mRenderData.rdMoveUp += 1;
	}
	if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_Q) == GLFW_PRESS) {
		mRenderData.rdMoveUp -= 1;
	}


	

}

void OGLRenderer::handleMouseButtonEvents(int button, int action, int mods)
{
	ImGuiIO& io = ImGui::GetIO();
	if (button >= 0 && button < ImGuiMouseButton_COUNT)
	{
		io.AddMouseButtonEvent(button, action == GLFW_PRESS);
	}
	if (io.WantCaptureMouse) return;

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		mMouseLock = !mMouseLock;
	}

	if (mMouseLock) glfwSetInputMode(mRenderData.rdWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else glfwSetInputMode(mRenderData.rdWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	
}

void OGLRenderer::handleMousePositionEvents(double xPos, double yPos)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddMousePosEvent((float)xPos, (float)yPos);
	if (io.WantCaptureMouse) return;

	int mouseMoveRelX = static_cast<int>(xPos) - mMouseXPos;
	int mouseMoveRelY = static_cast<int>(yPos) - mMouseYPos;

	if (mMouseLock)
	{
		mRenderData.rdViewAzimuth += mouseMoveRelX / 10.0f;
		if (mRenderData.rdViewAzimuth < 0.0) mRenderData.rdViewAzimuth += 360.0;
		if (mRenderData.rdViewAzimuth >= 360.0) mRenderData.rdViewAzimuth -= 360.0;

		mRenderData.rdViewElevation -= mouseMoveRelY / 10.0;
		if (mRenderData.rdViewElevation > 89.0) mRenderData.rdViewElevation = 89.0;
		if (mRenderData.rdViewElevation < -89.0) mRenderData.rdViewElevation = -89.0;
	}

	mMouseXPos = static_cast<int>(xPos);
	mMouseYPos = static_cast<int>(yPos);


}


void OGLRenderer::toggleVsync()
{
	int bool_Vsync = old_VSync ? 1 : 0;
	glfwSwapInterval(bool_Vsync);
	old_VSync = bool_Vsync;
}

void OGLRenderer::reorient_camera()
{
	mRenderData.rdCameraWorldPosition = glm::vec3(3.5, 2.5, 2.5);
	mRenderData.rdViewAzimuth = 300.0f;
	mRenderData.rdViewElevation = -15.0f;
	mRenderData.rdFieldOfView = 90;
	Logger::log(1, " re-oriented camera ");
}

OGLRenderer::OGLRenderer(GLFWwindow* window) {

	mRenderData.rdWindow = window;
	mViewMatrix = glm::mat4(1.0f);
	mProjectionMatrix = glm::mat4(1.0f);

}

void OGLRenderer::handleKeyEvents(int key, int scancode, int action, int mods)
{
	

	if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_ENTER) == GLFW_PRESS)
	{
		reorient_camera();
	}

	if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		isMove = !isMove;
		Logger::log(1, "can move objects : %s\n", isMove ? "true" : "false", __FUNCTION__);
	}
}

bool OGLRenderer::init(unsigned int width, unsigned int height)
{
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		return false;
	}
	if (!GLAD_GL_VERSION_4_6) { return false; }

	if (!mFramebuffer.init(width, height))
	{
		return false;
	}

	std::srand(static_cast<int>(time(NULL)));
	mRenderData.rdWidth = width;
	mRenderData.rdHeight = height;
	
	

	mVertexBuffer.init();

	size_t uniformMatrixBufferSize = 2 * sizeof(glm::mat4);
	mUniformBuffer.init(uniformMatrixBufferSize);
	Logger::log(1, "%s: matrix uniform buffer (size %i bytes) successfully created\n", __FUNCTION__, uniformMatrixBufferSize);

	if (!mGltfGPUShader.loadShaders("shaders/gltf_gpu.vert", "shaders/gltf_gpu.frag")){
		Logger::log(1, "%s: cannot find shaders\n",
			__FUNCTION__);
		return false;
	}

	if (!mCarShader.loadShaders("shaders/changed.vert", "shaders/changed.frag")){
		Logger::log(1, "%s: cannot find shaders\n",
			__FUNCTION__);
		return false;
	}

	if (!mDebugShader.loadShaders("shaders/debug.vert", "shaders/debug.frag")) {
		Logger::log(1, "%s: cannot find shaders\n",
			__FUNCTION__);
		return false;
	}

	if (!mGltfGPUShader.getuniformLocation("aModelStride"))
	{
		return false;
	}

	if (!mLineShader.loadShaders("shaders/line.vert", "shaders/line.frag"))
	{
		Logger::log(1, "%s : cannot find shaders\n", __FUNCTION__);
		return false;
	}
	mLineMesh = std::make_shared<OGLMesh>();


	if (!mGltfGPUDualQuatShader.loadShaders("shaders/gltf_gpu_dquat.vert", "shaders/gltf_gpu_dquat.frag"))
	{
		Logger::log(1, "%s: cannot find shaders\n",
			__FUNCTION__);
		return false;
	}

	if (!mGltfGPUDualQuatShader.getuniformLocation("aModelStride"))
	{
		return false;
	}

	

	mGltfModels.resize(3);

	mUniformBuffer.init(2 * sizeof(glm::mat4));
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);



	mUserInterface.init(mRenderData);

	
	glLineWidth(3.0);
	glDisable(GL_FRAMEBUFFER_SRGB);

	//mGltfModels.at(0) = std::make_shared<GltfModel>();
	//std::string modelFilename = "assets/Woman.gltf";
	//std::string modelTexFilename = "tex/Woman.png";
	//if (!mGltfModels.at(0)->loadModel(mRenderData, modelFilename, modelTexFilename))
	//{
	//	Logger::log(1, "%s: loading glTF model '%s' failed\n", __FUNCTION__, modelFilename.c_str());
	//	return false;
	//}
	//mGltfModels.at(0)->uploadVertexBuffers();
	//mGltfModels.at(0)->uploadIndexBuffer();

	//mGltfModels.at(1) = std::make_shared<GltfModel>();
	//modelTexFilename = "tex/Woman2.png";
	//if (!mGltfModels.at(1)->loadModel(mRenderData, modelFilename, modelTexFilename))
	//{
	//	Logger::log(1, "%s: loading glTF model '%s' failed\n", __FUNCTION__, modelFilename.c_str());
	//	return false;
	//}
	//mGltfModels.at(1)->uploadVertexBuffers();
	//mGltfModels.at(1)->uploadIndexBuffer();

	//mGltfModels.at(2) = std::make_shared<GltfModel>();
	//modelFilename = "assets/dq.gltf";
	//modelTexFilename = "tex/dq.png";
	//if (!mGltfModels.at(2)->loadModel(mRenderData, modelFilename, modelTexFilename)) {
	//	Logger::log(1, "%s: loading glTF model '%s' failed\n", __FUNCTION__, modelFilename.c_str());
	//	return false;
	//}
	//mGltfModels.at(2)->uploadVertexBuffers();
	//mGltfModels.at(2)->uploadIndexBuffer();


	// TEMPORARY : to limit the room for error for working with GPU instancing
	mGltfModel = std::make_shared<GltfModel>();
	std::string modelFilename = "assets/Woman.gltf";
	std::string modelTexFilename = "tex/Woman.png";
	bool useMeshPrimitiveApproach = false;
	if (!mGltfModel->loadModel(mRenderData, modelFilename, modelTexFilename, useMeshPrimitiveApproach, true)) {
		Logger::log(1, "%s: loading glTF model '%s' failed\n", __FUNCTION__, modelFilename.c_str());
		return false;
	}
	mGltfModel->uploadVertexBuffers();
	mGltfModel->uploadIndexBuffer();
	

	useMeshPrimitiveApproach = true;
	modelTexFilename = "tex/Woman2.png"; // change to texture 2 for verifying the result later
	mGltfModel1 = std::make_shared<GltfModel>();
	if (!mGltfModel1->loadModel(mRenderData, modelFilename, modelTexFilename, useMeshPrimitiveApproach, false)) {
		Logger::log(1, "%s: loading glTF model '%s' failed\n", __FUNCTION__, modelFilename.c_str());
		return false;
	}
	mFirstLogRender = true;
	mGltfModel1->setWorldPosition(glm::vec3(0.0, 0.0, 0.0));
	mGltfModel1->setWorldRotation(glm::vec3(0.0));
	mGltfModel1->setDebugModelScale(100.0f);

	

	mGltfModel2 = std::make_shared<GltfModel>();
	modelFilename = "assets/CesiumMilkTruck.glb";
	modelTexFilename = "";
	if (!mGltfModel2->loadModel(mRenderData, modelFilename, modelTexFilename, useMeshPrimitiveApproach, false)) {
		Logger::log(1, "%s: loading glTF model '%s' failed\n", __FUNCTION__, modelFilename.c_str());
		return false;
	}
	

	

	int numTriangles = 0;

	for (int i = 0; i < 2; ++i) {
		int xPos = std::rand() % 40 - 20;
		int zPos = std::rand() % 40 - 20;
		mGltfInstances.emplace_back(std::make_shared<GltfInstance>(mGltfModel, glm::vec2(static_cast<float>(xPos),
			static_cast<float>(zPos)), true));
		numTriangles += mGltfModel->getTriangleCount();
	}

	
	//for (int i = 0; i < 100; ++i) {
	//	int xPos = std::rand() % 40 - 20;
	//	int zPos = std::rand() % 40 - 20;
	//	int modelNo = std::rand() % 2;
	//	mGltfInstances.emplace_back(std::make_shared<GltfInstance>(mGltfModels.at(modelNo), glm::vec2(static_cast<float>(xPos),
	//		static_cast<float>(zPos)), true));
	//	numTriangles += mGltfModels.at(modelNo)->getTriangleCount();
	//}

	//for (int i = 0; i < 25; ++i) {
	//	int xPos = std::rand() % 50 - 25;
	//	int zPos = std::rand() % 20 - 50;
	//	mGltfInstances.emplace_back(std::make_shared<GltfInstance>(mGltfModels.at(2), glm::vec2(static_cast<float>(xPos),
	//		static_cast<float>(zPos)), true));
	//	numTriangles += mGltfModels.at(2)->getTriangleCount();
	//}

	mRenderData.rdTriangleCount = numTriangles;

	mRenderData.rdNumberOfInstances = mGltfInstances.size();

	size_t modelJointMatrixBufferSize = 0;
	size_t modelJointDualQuatBufferSize = 0;
	int jointMatrixSize = 0;
	int jointQuatSize = 0;

	for (const auto& instance : mGltfInstances)
	{
		jointMatrixSize += instance->getJointMatrixSize();
		modelJointMatrixBufferSize += instance->getJointMatrixSize() * sizeof(glm::mat4);

		jointQuatSize = instance->getJointDualQuatsSize();
		modelJointDualQuatBufferSize += instance->getJointDualQuatsSize() * sizeof(glm::mat2x4);
	}
	//
	//mGltfShaderStorageBuffer.init(modelJointMatrixBufferSize);
	//Logger::log(1, "%s: glTF joint matrix shader storage buffer (size %i bytes) successfully created\n", __FUNCTION__, modelJointMatrixBufferSize);

	mGltfTextureBuffer.init(modelJointMatrixBufferSize);
	Logger::log(1, "%s: glTF joint matrix texture buffer (size %i bytes) successfully created\n", __FUNCTION__, modelJointMatrixBufferSize);

	mGltfDualQuatSSBuffer.init(modelJointDualQuatBufferSize);
	Logger::log(1, "%s: glTF joint dual quaternions shader storage buffer (size %i bytes) successfully created\n", __FUNCTION__, modelJointDualQuatBufferSize);

	mLineMesh = std::make_shared<OGLMesh>();
	Logger::log(1, "%s: line mesh storage initialized\n", __FUNCTION__);

	mFrameTimer.start();
	return true;

}

void OGLRenderer::setSize(unsigned int width, unsigned int height)
{
	mFramebuffer.resize(width, height);
	glViewport(0, 0, width, height);
}

void OGLRenderer::uploadData(OGLMesh vertexData)
{
	mRenderData.rdTriangleCount = vertexData.vertices.size();
	mVertexBuffer.uploadData(vertexData);
}


void OGLRenderer::setModelMatrix(const glm::mat4& model)
{
	
}


void OGLRenderer::draw() {
	/* handle minimize */
	while (mRenderData.rdWidth == 0 || mRenderData.rdHeight == 0) {
		glfwGetFramebufferSize(mRenderData.rdWindow, &mRenderData.rdWidth, &mRenderData.rdHeight);
		glfwWaitEvents();
	}

	/* get time difference for movement */
	double tickTime = glfwGetTime();
	mRenderData.rdTickDiff = tickTime - mLastTickTime;

	mRenderData.rdFrameTime = mFrameTimer.stop();
	mFrameTimer.start();

	handleMovementKeys();

	/* draw to framebuffer */
	mFramebuffer.bind();

	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mMatrixGenerateTimer.start();
	mProjectionMatrix = glm::perspective(
		glm::radians(static_cast<float>(mRenderData.rdFieldOfView)),
		static_cast<float>(mRenderData.rdWidth) / static_cast<float>(mRenderData.rdHeight),
		0.01f, 500.0f);

	mViewMatrix = mCamera.getViewMatrix(mRenderData);

	/* animate and update inverse kinematics */
	mRenderData.rdIKTime = 0.0f;
	for (auto& instance : mGltfInstances) {
		instance->updateAnimation();

		mIKTimer.start();
		instance->solveIK();
		mRenderData.rdIKTime += mIKTimer.stop();
	}

	/* save value to avoid changes during later call */
	int selectedInstance = mRenderData.rdCurrentSelectedInstance;
	glm::vec2 modelWorldPos = mGltfInstances.at(selectedInstance)->getWorldPosition();
	glm::quat modelWorldRot = mGltfInstances.at(selectedInstance)->getWorldRotation();

	mLineMesh->vertices.clear();

	/* get gltTF skeleton */
	mSkeletonLineIndexCount = 0;
	for (const auto& instance : mGltfInstances) {
		ModelSettings settings = instance->getInstanceSettings();
		if (settings.msDrawSkeleton) {
			std::shared_ptr<OGLMesh> mesh = instance->getSkeleton();
			mSkeletonLineIndexCount += mesh->vertices.size();
			mLineMesh->vertices.insert(mLineMesh->vertices.begin(),
				mesh->vertices.begin(), mesh->vertices.end());
		}
	}

	/* get coordinate arrows for the IK target of current instance only */
	mCoordArrowsLineIndexCount = 0;
	{
		ModelSettings ikSettings = mGltfInstances.at(selectedInstance)->getInstanceSettings();
		if (ikSettings.msIkMode == ikMode::ccd ||
			ikSettings.msIkMode == ikMode::fabrik) {
			mCoordArrowsMesh = mCoordArrowsModel.getVertexData();
			mCoordArrowsLineIndexCount += mCoordArrowsMesh.vertices.size();
			std::for_each(mCoordArrowsMesh.vertices.begin(), mCoordArrowsMesh.vertices.end(),
				[=](auto& n) {
					n.color /= 2.0f;
					n.position = modelWorldRot * n.position;
					n.position += ikSettings.msIkTargetWorldPos;
				});

			mLineMesh->vertices.insert(mLineMesh->vertices.end(),
				mCoordArrowsMesh.vertices.begin(), mCoordArrowsMesh.vertices.end());
		}
	}

	/* draw coordiante arrows*/
	mCoordArrowsMesh = mCoordArrowsModel.getVertexData();
	mCoordArrowsLineIndexCount += mCoordArrowsMesh.vertices.size();
	std::for_each(mCoordArrowsMesh.vertices.begin(), mCoordArrowsMesh.vertices.end(),
		[=](auto& n) {
			n.color /= 2.0f;
			n.position = modelWorldRot * n.position;
			n.position += glm::vec3(modelWorldPos.x, 0.0f, modelWorldPos.y);
		});

	mLineMesh->vertices.insert(mLineMesh->vertices.end(),
		mCoordArrowsMesh.vertices.begin(), mCoordArrowsMesh.vertices.end());

	mRenderData.rdMatrixGenerateTime = mMatrixGenerateTimer.stop();

	mUploadToUBOTimer.start();
	std::vector<glm::mat4> matrixData;
	matrixData.push_back(mViewMatrix);
	matrixData.push_back(mProjectionMatrix);
	matrixData.clear();
	mUniformBuffer.uploadUboData(matrixData, 0);

	mModelJointMatrices.clear();
	mModelJointDualQuats.clear();

	mGltfMatrixInstances.clear();
	mGltfDQInstances.clear();
	unsigned int numTriangles = 0;
	unsigned int matrixInstances = 0;
	unsigned int dualQuatInstances = 0;

	for (const auto& instance : mGltfInstances) {
		ModelSettings settings = instance->getInstanceSettings();
		if (!settings.msDrawModel) {
			continue;
		}

		if (settings.msVertexSkinningMode == skinningMode::dualQuat) {
			std::vector<glm::mat2x4> quats = instance->getJointDualQuats();
			mModelJointDualQuats.insert(mModelJointDualQuats.end(),
				quats.begin(), quats.end());
			++dualQuatInstances;
		}
		else {
			std::vector<glm::mat4> mats = instance->getJointMatrices();
			mModelJointMatrices.insert(mModelJointMatrices.end(),
				mats.begin(), mats.end());
			++matrixInstances;
		}
		numTriangles += instance->getModel()->getTriangleCount();
	}

	mRenderData.rdTriangleCount = numTriangles;

	mGltfTextureBuffer.uploadTboData(mModelJointMatrices, 1);
	mGltfDualQuatSSBuffer.uploadSsboData(mModelJointDualQuats, 2);

	mRenderData.rdUploadToUBOTime = mUploadToUBOTimer.stop();

	/* upload vertex data */
	mUploadToVBOTimer.start();

	uploadData(*mLineMesh);

	mRenderData.rdUploadToVBOTime = mUploadToVBOTimer.stop();

	mGltfGPUShader.use();

	mGltfTextureBuffer.bind();

	mGltfGPUShader.setUniformValue(mGltfInstances.at(0)->getJointMatrixSize());
	mGltfModel->drawInstanced(matrixInstances);



	mGltfGPUDualQuatShader.use();
	mGltfGPUDualQuatShader.setUniformValue(mGltfInstances.at(0)->getJointDualQuatsSize());
	mGltfModel->drawInstanced(dualQuatInstances);

	/* draw the coordinate arrow WITH depth buffer */
	if (mCoordArrowsLineIndexCount > 0) {
		mLineShader.use();
		mVertexBuffer.bindAndDraw(GL_LINES, mSkeletonLineIndexCount, mCoordArrowsLineIndexCount);
	}

	/* draw the skeleton, disable depth test to overlay */
	if (mSkeletonLineIndexCount > 0) {
		glDisable(GL_DEPTH_TEST);
		mLineShader.use();
		mVertexBuffer.bindAndDraw(GL_LINES, 0, mSkeletonLineIndexCount);
		glEnable(GL_DEPTH_TEST);
	}
	glDisable(GL_CULL_FACE);
	//mCarShader.use();
	//
	//matrixData.push_back(mViewMatrix);
	//matrixData.push_back(mProjectionMatrix);
	//mUniformBuffer.uploadUboData(matrixData, 0);
	//matrixData.clear();
	//mGltfModel2->drawNodeApproach(mCarShader, false);

	mDebugShader.use();

	matrixData.push_back(mViewMatrix);
	matrixData.push_back(mProjectionMatrix);
	mUniformBuffer.uploadUboData(matrixData, 0);
	matrixData.clear();
	if (mFirstLogRender)
	{
		mGltfModel1->drawNodeApproach(mDebugShader, true);
		mFirstLogRender = false;
	}
	else mGltfModel1->drawNodeApproach(mDebugShader, false);
	
	

	/* blit color buffer to screen */
	mFramebuffer.drawToScreen();
	mFramebuffer.unbind();
	mUIGenerateTimer.start();

	ModelSettings settings = mGltfInstances.at(selectedInstance)->getInstanceSettings();
	mUserInterface.createFrame(mRenderData, settings);
	mGltfInstances.at(selectedInstance)->setInstanceSettings(settings);
	mGltfInstances.at(selectedInstance)->checkForUpdates();

	mRenderData.rdUIGenerateTime = mUIGenerateTimer.stop();

	mUIDrawTimer.start();
	mUserInterface.render();
	mRenderData.rdUIDrawTime = mUIDrawTimer.stop();

	mLastTickTime = tickTime;
}

void OGLRenderer::cleanup()
{
	mUserInterface.cleanup();
	
	mCarShader.cleanup();

	mGltfGPUDualQuatShader.cleanup();

}
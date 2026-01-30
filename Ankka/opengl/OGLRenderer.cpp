#include "opengl/OGLRenderer.h"
#include "Ankka/Logger.h"
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

	if (isMove)
	{
		glm::vec3 h = rc.screenToWorld(xPos, yPos, mRenderData.rdCameraWorldPosition.z, mRenderData.rdWindow, mCamera.getViewMatrix(mRenderData), mProjectionMatrix);
		mGltfModel->modelMatrix()[3] = glm::vec4(h, 1.0f);
	}

}


void OGLRenderer::toggleVsync()
{
	int bool_Vsync = mRenderData.isVSYNC ? 1 : 0;
	glfwSwapInterval(bool_Vsync);
	old_VSync = bool_Vsync;
}

void OGLRenderer::reorient_camera()
{
	mRenderData.rdCameraWorldPosition = glm::vec3(3.5, 2.5, 2.5);
	mRenderData.rdViewAzimuth = 300.0f;
	mRenderData.rdViewElevation = -15.0f;
	mRenderData.rdFielfOfView = 90;
	Logger::log(1, " re-oriented camera ");
}

OGLRenderer::OGLRenderer(GLFWwindow* window) {

	mRenderData.rdWindow = window;
	mViewMatrix = glm::mat4(1.0f);
	mProjectionMatrix = glm::mat4(1.0f);

}

void OGLRenderer::handleKeyEvents(int key, int scancode, int action, int mods)
{
	if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		mRenderData.rdUseChangedShader = !mRenderData.rdUseChangedShader;
		Logger::log(1, "using shader : %s\n",
			mRenderData.rdUseChangedShader ? "changed" : "normal",
			__FUNCTION__);
	}

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
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		return false;
	}
	if (!GLAD_GL_VERSION_4_6) { return false; }

	if (!mFramebuffer.init(width, height))
	{
		return false;
	}

	
	if (!mTex.loadTexture("tex/crate.png", true)) {
		Logger::log(1, "%s: cannot find texture", __FUNCTION__);
		return false;
	}

	mVertexBuffer.init();

	if (!mBasicShader.loadShaders("shaders/basic.vert", "shaders/basic.frag"))
	{
		Logger::log(1, "%s: cannot find shaders\n",
			__FUNCTION__);
		return false;
	}

	if (!mChangedShader.loadShaders("shaders/changed.vert", "shaders/changed.frag"))
	{
		Logger::log(1, "% s: cannot find shaders\n", __FUNCTION__);
		return false;
	}

	if (!mGltfShader.loadShaders("shaders/gltf.vert", "shaders/gltf.frag"))
	{
		Logger::log(1, "%s: cannot find shaders\n",
			__FUNCTION__);
		return false;
	}

	mUniformBuffer.init();
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	mRenderData.rdWidth = width;
	mRenderData.rdHeight = height;

	mUserInterface.init(mRenderData);

	mGltfModels.reserve(2 * sizeof(GltfModel));

	mGltfModel = std::make_shared<GltfModel>();
	std::string modelFilename = "assets/Woman.gltf";
	std::string modelTexFilename = "tex/Woman.png";

	if (!mGltfModel->loadModel(mRenderData, modelFilename, modelTexFilename))
	{
		return false;
	}
	mGltfModel->uploadIndexBuffer();

	mGltfModel->uploadVertexBuffers();
	mGltfModel1 = std::make_shared<GltfModel>();
	std::string modelFilename1 = "assets/DamagedHelmet.glb";
	
	if (!mGltfModel1->loadModel(mRenderData, modelFilename1, modelTexFilename))
	{
		return false;
	}

	mGltfModel1->uploadIndexBuffer();

	mGltfModel1->uploadVertexBuffers();
	return true;

}

void OGLRenderer::setSize(unsigned int width, unsigned int height)
{
	mFramebuffer.resize(width, height);
	glViewport(0, 0, width, height);
}

void OGLRenderer::uploadData(OGLMesh vertexData)
{
	mRenderData.rdTriangelCount = vertexData.vertices.size();
	mVertexBuffer.uploadData(vertexData);
}


void OGLRenderer::draw()
{
	double ticktime = glfwGetTime();
	mRenderData.rdTickDiff = ticktime - lastTickTime;

	static float prevFrameStartTime = 0.0f;
	float frameStartTime = glfwGetTime();

	if (mRenderData.isVSYNC != old_VSync)
	{
		toggleVsync();
	}

	mFramebuffer.bind();
	glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mProjectionMatrix = glm::perspective(
		static_cast<float>(mRenderData.rdFielfOfView),
		static_cast<float>(mRenderData.rdWidth) / static_cast<float>(mRenderData.rdHeight),
		0.01f,
		120.0f);

	float t = glfwGetTime();

	//glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 model = glm::mat4(1.0f);
	
	if (mRenderData.rdUseChangedShader)
	{	
		mChangedShader.use();
		model = glm::rotate(glm::mat4(1.0f), -t, glm::vec3(0.0f, 0.0f, 1.0f));
		mChangedShader.setM4_Uniform("model", model);
	}
	else
	{
		mBasicShader.use();
		model = glm::rotate(glm::mat4(1.0f), t, glm::vec3(0.0f, 0.0f, 1.0f));
		mChangedShader.setM4_Uniform("model", model);
	}

	mViewMatrix = mCamera.getViewMatrix(mRenderData);
	mUniformBuffer.uploadUboData(mCamera.getViewMatrix(mRenderData), mProjectionMatrix);
	
	mTex.bind();
	mVertexBuffer.bind();
	mVertexBuffer.draw(GL_TRIANGLES, 0, mRenderData.rdTriangelCount * 3);
	mVertexBuffer.unbind();
	
	mViewMatrix = mCamera.getViewMatrix(mRenderData) * glm::mat4(1.0f);
	mUniformBuffer.uploadUboData(mViewMatrix, mProjectionMatrix);

	mGltfShader.use();
	mGltfShader.setM4_Uniform("model", mGltfModel->modelMatrix());
	mGltfModel->draw();
	

	mGltfShader.setM4_Uniform("model", mGltfModel1->modelMatrix());
	mGltfModel1->draw();

	mTex.unbind();
	mFramebuffer.unbind();

	mFramebuffer.drawToScreen();
	mUIGenerateTimer.start();

	handleMovementKeys();

	mUserInterface.createFrame(mRenderData);
	mUserInterface.render();
	mRenderData.rdUIGenerateTime = mUIGenerateTimer.stop();
	mRenderData.rdFrameTime = frameStartTime - prevFrameStartTime;
	prevFrameStartTime = frameStartTime;

	lastTickTime = ticktime;
}

void OGLRenderer::cleanup()
{
	mUserInterface.cleanup();
	mBasicShader.cleanup();
	mChangedShader.cleanup();

	mGltfModel->cleanup();
	mGltfModel.reset();
	mGltfShader.cleanup();

}
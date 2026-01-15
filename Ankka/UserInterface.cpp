#include <string>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Ankka/UserInterface.h"

void UserInterface::init(OGLRenderData& renderData)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(renderData.rdWindow, true);
	const char* glslVersion = "#version 460 core";
	ImGui_ImplOpenGL3_Init(glslVersion);
}

void UserInterface::cleanup()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void UserInterface::createFrame(OGLRenderData& renderData)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiWindowFlags imguiWindowFlags  = 0;
	ImGui::SetNextWindowBgAlpha(0.8f);
	ImGui::Begin("Control", nullptr, imguiWindowFlags);

	static float newFps = 0.0f;
	if (renderData.rdFrameTime > 0.0)
	{
		newFps = 1.0f / renderData.rdFrameTime;
	}

	framesPerSecond = (averagingAlpha * framesPerSecond) + (1.0f - averagingAlpha) * newFps;

	ImGui::Text("FPS:");
	ImGui::SameLine();
	std::string frame_str = std::to_string(framesPerSecond);
	std::string trimString = frame_str.substr(0, frame_str.length() - 4);

	ImGui::Text(trimString.c_str());
	ImGui::Separator();

	ImGui::Text("Triangles:");
	ImGui::SameLine();
	ImGui::Text(std::to_string(renderData.rdTriangelCount).c_str());

	std::string windowDims = std::to_string(renderData.rdWidth) + "x"
		+ std::to_string(renderData.rdHeight) + "y";
	ImGui::Text("Window dimensions:");
	ImGui::SameLine();
	ImGui::Text(windowDims.c_str());

	std::string imgWindowPos
		= std::to_string(static_cast<int>(ImGui::GetWindowPos().x)) + "/"
		+ std::to_string(static_cast<int>(ImGui::GetWindowPos().y));

	ImGui::Text("ImGui Window Position :");
	ImGui::SameLine();
	ImGui::Text(imgWindowPos.c_str());

	ImGui::Text("UI Generation time:");
	ImGui::SameLine();
	ImGui::Text(std::to_string(renderData.rdUIGenerateTime).c_str());
	ImGui::SameLine();
	ImGui::Text("ms");

	ImGui::End();
}

void UserInterface::render()
{
	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
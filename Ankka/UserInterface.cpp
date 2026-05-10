#include <string>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
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

static std::string trim_float(float f, int l)
{
	std::string s = std::to_string(f);
	return s.substr(0, s.length() - l);
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
	ImGui::Text(trim_float(framesPerSecond, 4).c_str());
	ImGui::Separator();

	ImGui::Text("Triangles:");
	ImGui::SameLine();
	ImGui::Text(std::to_string(renderData.rdTriangelCount + renderData.rdGltfTriangleCount).c_str());

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
	ImGui::Text(trim_float(renderData.rdUIGenerateTime, 4).c_str());
	ImGui::SameLine();
	ImGui::Text("ms");

	ImGui::Checkbox("Vsync", &renderData.isVSYNC);

	if (checkBoxChecked)
	{
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 2550, 2, 255));
		ImGui::Text("VSYNC is on");
		ImGui::PopStyleColor();
		renderData.isVSYNC = !renderData.isVSYNC;
	}
	

	if (ImGui::Button("Toggle Shader"))
	{
		renderData.rdUseChangedShader = !renderData.rdUseChangedShader;
	}
	ImGui::SameLine();
	std::string shader_string = renderData.rdUseChangedShader ? "Changed Shader" : "Basic Shader";
	ImGui::Text(shader_string.c_str());

	ImGui::Text("Field of view");
	ImGui::SameLine();
	ImGui::SliderInt("##FOX", &renderData.rdFielfOfView, 40, 150);

	ImGui::Text("View azi:");
	ImGui::SameLine();
	ImGui::Text("%s", trim_float(renderData.rdViewAzimuth, 4).c_str());

	ImGui::Text("View elev::");
	ImGui::SameLine();
	ImGui::Text("%s", trim_float(renderData.rdViewElevation, 4).c_str());
	ImGui::Separator();

	ImGui::Text("Camera Position");
	ImGui::SameLine();
	ImGui::Text("%s", glm::to_string(renderData.rdCameraWorldPosition).c_str());

	ImGuiSliderFlags flags = ImGuiSliderFlags_ClampOnInput;

	if (ImGui::CollapsingHeader("glTF Animation Blending")) {
		ImGui::Checkbox("Blending Type:", &renderData.rdCrossBlending);
		ImGui::SameLine();
		if (renderData.rdCrossBlending) {
			ImGui::Text("Cross");
		}
		else {
			ImGui::Text("Single");
		}

		if (renderData.rdCrossBlending) {
			ImGui::BeginDisabled();
		}

		ImGui::Text("Blend Factor");
		ImGui::SameLine();
		ImGui::SliderFloat("##BlendFactor", &renderData.rdAnimBlendFactor, 0.0f, 1.0f, "%.3f",
			flags);

		if (renderData.rdCrossBlending) {
			ImGui::EndDisabled();
		}

		if (!renderData.rdCrossBlending) {
			ImGui::BeginDisabled();
		}

		ImGui::Text("Dest Clip   ");
		ImGui::SameLine();
		ImGui::SliderInt("##DestClip", &renderData.rdCrossBlendDestAnimClip, 0,
			renderData.rdAnimClipSize - 1, "%d", flags);

		ImGui::Text("Dest Clip Name: %s", renderData.rdCrossBlendDestClipName.c_str());

		ImGui::Text("Cross Blend ");
		ImGui::SameLine();
		ImGui::SliderFloat("##CrossBlendFactor", &renderData.rdAnimCrossBlendFactor, 0.0f, 1.0f,
			"%.3f", flags);

		if (!renderData.rdCrossBlending) {
			ImGui::EndDisabled();
		}
	}

	ImGui::End();
}

void UserInterface::render()
{
	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
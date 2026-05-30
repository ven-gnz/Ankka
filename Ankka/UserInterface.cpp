#include <string>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

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
    mFPSValues.resize(mNumFPSValues);
}

    ImGui::CreateContext();

    ImGui_ImplGlfw_InitForOpenGL(renderData.rdWindow, true);

    const char* glslVersion = "#version 460 core";
    ImGui_ImplOpenGL3_Init(glslVersion);

    ImGui::StyleColorsDark();

    /* init plot vectors */
    mFPSValues.resize(mNumFPSValues);
    mFrameTimeValues.resize(mNumFrameTimeValues);
    mModelUploadValues.resize(mNumModelUploadValues);
    mMatrixGenerationValues.resize(mNumMatrixGenerationValues);
    mIKValues.resize(mNumIKValues);
    mMatrixUploadValues.resize(mNumMatrixUploadValues);
    mUiGenValues.resize(mNumUiGenValues);
    mUiDrawValues.resize(mNumUiDrawValues);
}

void UserInterface::createFrame(OGLRenderData& renderData)
{

    static double updateTime = 0.0;
    if (updateTime < 0.000001)
    {
        updateTime = ImGui::GetTime();
    }
    static int fpsOffset = 0;

    while (updateTime < ImGui::GetTime())
    {
        mFPSValues.at(fpsOffset) = mFramesPerSecond;
        fpsOffset = ++fpsOffset & mNumFPSValues;
        updateTime += 1.0 / 30.0;
    }
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

    mFramesPerSecond = (averagingAlpha * mFramesPerSecond) + (1.0f - averagingAlpha) * newFps;

    ImGui::BeginGroup();
    ImGui::Text("FPS:");
    ImGui::SameLine();
    ImGui::Text("%s", std::to_string(mFramesPerSecond).c_str());
    ImGui::EndGroup();

    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        float averageFPS = 0.0f;
        for (const auto value : mFPSValues) {
            averageFPS += value;
        }
        averageFPS /= static_cast<float>(mNumFPSValues);
        std::string fpsOverlay = "now:     " + std::to_string(mFramesPerSecond) + "\n30s avg: " + std::to_string(averageFPS);
        ImGui::Text("FPS");
        ImGui::SameLine();
        ImGui::PlotLines("##FrameTimes", mFPSValues.data(), mFPSValues.size(), fpsOffset, fpsOverlay.c_str(), 0.0f, FLT_MAX,
            ImVec2(0, 80));
        ImGui::EndTooltip();
    }

    if (ImGui::CollapsingHeader("Info")) {
        ImGui::Text("Triangles:");
        ImGui::SameLine();
        ImGui::Text("%s", std::to_string(renderData.rdTriangelCount + renderData.rdGltfTriangleCount).c_str());

        std::string windowDims = std::to_string(renderData.rdWidth) + "x" + std::to_string(renderData.rdHeight);
        ImGui::Text("Window Dimensions:");
        ImGui::SameLine();
        ImGui::Text("%s", windowDims.c_str());

        std::string imgWindowPos = std::to_string(static_cast<int>(ImGui::GetWindowPos().x)) + "/" + std::to_string(static_cast<int>(ImGui::GetWindowPos().y));
        ImGui::Text("ImGui Window Position:");
        ImGui::SameLine();
        ImGui::Text("%s", imgWindowPos.c_str());
    }

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

    if (ImGui::CollapsingHeader("glTF Animation")) {
        ImGui::Checkbox("Play Animation", &renderData.rdPlayAnimation);

        if (!renderData.rdPlayAnimation) {
            ImGui::BeginDisabled();
        }

        ImGui::Text("Animation Direction:");
        ImGui::SameLine();
        if (ImGui::RadioButton("Forward",
            renderData.rdAnimationPlayDirection == replayDirection::forward)) {
            renderData.rdAnimationPlayDirection = replayDirection::forward;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Backward",
            renderData.rdAnimationPlayDirection == replayDirection::backward)) {
            renderData.rdAnimationPlayDirection = replayDirection::backward;
        }

        if (!renderData.rdPlayAnimation) {
            ImGui::EndDisabled();
        }

        ImGui::Text("Clip   ");
        ImGui::SameLine();
        if (ImGui::BeginCombo("##ClipCombo",
            renderData.rdClipNames.at(renderData.rdAnimClip).c_str())) {
            for (int i = 0; i < renderData.rdClipNames.size(); ++i) {
                const bool isSelected = (renderData.rdAnimClip == i);
                if (ImGui::Selectable(renderData.rdClipNames.at(i).c_str(), isSelected)) {
                    renderData.rdAnimClip = i;
                }

                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (renderData.rdPlayAnimation) {
            ImGui::Text("Speed  ");
            ImGui::SameLine();
            ImGui::SliderFloat("##ClipSpeed", &renderData.rdAnimSpeed, 0.0f, 2.0f, "%.3f", flags);
        }
        else {
            ImGui::Text("Timepos");
            ImGui::SameLine();
            ImGui::SliderFloat("##ClipPos", &renderData.rdAnimTimePosition, 0.0f,
                renderData.rdAnimEndTime, "%.3f", flags);
        }
    }

    if (ImGui::CollapsingHeader("glTF Animation Blending")) {
        ImGui::Text("Blending Type:");
        ImGui::SameLine();
        if (ImGui::RadioButton("Fade In/Out",
            renderData.rdBlendingMode == blendMode::fadeinout)) {
            renderData.rdBlendingMode = blendMode::fadeinout;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Crossfading",
            renderData.rdBlendingMode == blendMode::crossfade)) {
            renderData.rdBlendingMode = blendMode::crossfade;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Additive",
            renderData.rdBlendingMode == blendMode::additive)) {
            renderData.rdBlendingMode = blendMode::additive;
        }

        if (renderData.rdBlendingMode == blendMode::fadeinout) {
            ImGui::Text("Blend Factor");
            ImGui::SameLine();
            ImGui::SliderFloat("##BlendFactor", &renderData.rdAnimBlendFactor, 0.0f, 1.0f, "%.3f",
                flags);
        }

        if (renderData.rdBlendingMode == blendMode::crossfade ||
            renderData.rdBlendingMode == blendMode::additive) {
            ImGui::Text("Dest Clip   ");
            ImGui::SameLine();
            if (ImGui::BeginCombo("##DestClipCombo",
                renderData.rdClipNames.at(renderData.rdCrossBlendDestAnimClip).c_str())) {
                for (int i = 0; i < renderData.rdClipNames.size(); ++i) {
                    const bool isSelected = (renderData.rdCrossBlendDestAnimClip == i);
                    if (ImGui::Selectable(renderData.rdClipNames.at(i).c_str(), isSelected)) {
                        renderData.rdCrossBlendDestAnimClip = i;
                    }

                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::Text("Cross Blend ");
            ImGui::SameLine();
            ImGui::SliderFloat("##CrossBlendFactor", &renderData.rdAnimCrossBlendFactor, 0.0f, 1.0f,
                "%.3f", flags);
        }

        if (renderData.rdBlendingMode == blendMode::additive) {
            ImGui::Text("Split Node  ");
            ImGui::SameLine();
            if (ImGui::BeginCombo("##SplitNodeCombo",
                renderData.rdSkelSplitNodeNames.at(renderData.rdSkelSplitNode).c_str())) {
                for (int i = 0; i < renderData.rdSkelSplitNodeNames.size(); ++i) {
                    if (renderData.rdSkelSplitNodeNames.at(i).compare("(invalid)") != 0) {
                        const bool isSelected = (renderData.rdSkelSplitNode == i);
                        if (ImGui::Selectable(renderData.rdSkelSplitNodeNames.at(i).c_str(), isSelected)) {
                            renderData.rdSkelSplitNode = i;
                        }

                        if (isSelected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                }
                ImGui::EndCombo();
            }
        }

    }

    ImGui::End();
}

void UserInterface::render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UserInterface::cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

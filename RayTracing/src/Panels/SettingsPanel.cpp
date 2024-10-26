#include "SettingsPanel.h"

#include "Walnut/UI/UI.h"

SettingsPanel::SettingsPanel(Renderer& renderer, bool& showSettingsPanel)
	: m_Renderer(renderer), m_ShowSettingsPanel(showSettingsPanel)
{}

bool SettingsPanel::OnUIRender() {
	bool resetFrameIndex = false;

	if (m_ShowSettingsPanel) {

		ImGui::Begin("Settings", &m_ShowSettingsPanel);
		ImGui::BeginChild("Boolean Settings", ImVec2(0, 128), true);
		ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);
		ImGui::Checkbox("Multithreading", &m_Renderer.GetSettings().Multithreading);
		ImGui::Checkbox("Fast Random", &m_Renderer.GetSettings().PCHRandom);
		ImGui::EndChild();

		if (m_Renderer.GetSettings().PCHRandom) {
			ImGui::BeginChild("PCH Settings", ImVec2(0, 128), true);
			ImGui::Checkbox("Use Clock Time", &m_Renderer.GetSettings().UseClockTime);
			ImGui::Checkbox("Use Frame Index", &m_Renderer.GetSettings().UseFrameIndex);
			ImGui::Checkbox("Use Ray Bounces", &m_Renderer.GetSettings().UseRayBounces);
			ImGui::EndChild();
		}

		ImGui::BeginChild("Slider Settings", ImVec2(0, 90), true);
		ImGui::DragInt("Ray Bounces", &m_Renderer.GetSettings().RayBounces, 1, 1, std::numeric_limits<int>::max());
		ImGui::SliderInt("Resolution Scale", &m_Renderer.GetSettings().ResolutionScale, 1, 100, "%d%%", ImGuiSliderFlags_AlwaysClamp);
		ImGui::EndChild();

		if (ImGui::Button("Reset Accumulation", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			resetFrameIndex = true;
		}
		ImGui::End();
	}

	return resetFrameIndex;
}

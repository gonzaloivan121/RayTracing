#include "SettingsPanel.h"

#include "Walnut/UI/UI.h"

SettingsPanel::SettingsPanel(Renderer& renderer, bool& showSettingsPanel)
	: m_Renderer(renderer), m_ShowSettingsPanel(showSettingsPanel)
{}

bool SettingsPanel::OnUIRender() {
	bool resetFrameIndex = false;

	if (m_ShowSettingsPanel) {
		ImGui::Begin("Settings", &m_ShowSettingsPanel);
		ImGui::BeginChild("Settings", ImVec2(0, 204), true);
		ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);
		ImGui::Checkbox("Multithreading", &m_Renderer.GetSettings().Multithreading);
		ImGui::Checkbox("PCH Random", &m_Renderer.GetSettings().PCHRandom);
		ImGui::DragInt("Ray Bounces", &m_Renderer.GetSettings().RayBounces, 1, 1, std::numeric_limits<int>::max());
		ImGui::SliderInt("Resolution Scale", &m_Renderer.GetSettings().ResolutionScale, 1, 100, "%d%%", ImGuiSliderFlags_AlwaysClamp);
		ImGui::EndChild();

		if (ImGui::Button("Reset Accumulation")) {
			resetFrameIndex = true;
		}
		ImGui::End();
	}

	return resetFrameIndex;
}

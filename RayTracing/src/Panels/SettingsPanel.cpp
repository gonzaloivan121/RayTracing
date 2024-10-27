#include "SettingsPanel.h"

#include "Walnut/UI/UI.h"

#include "../Renderer/Serializer/RendererSettingsSerializer.h"

SettingsPanel::SettingsPanel(Renderer& renderer, bool& showSettingsPanel)
	: m_Renderer(renderer), m_ShowSettingsPanel(showSettingsPanel)
{
	LoadRendererSettings();
}

bool SettingsPanel::OnUIRender() {
	bool resetFrameIndex = false;

	if (m_ShowSettingsPanel) {
		LoadRendererSettings();

		ImGui::Begin("Settings", &m_ShowSettingsPanel);

		ImGui::BeginChild("Boolean Settings", ImVec2(0, 128), true);
		ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);
		ImGui::Checkbox("Multithreading", &m_Renderer.GetSettings().Multithreading);
		ImGui::Checkbox("Fast Random", &m_Renderer.GetSettings().FastRandom);
		ImGui::EndChild();

		if (m_Renderer.GetSettings().FastRandom) {
			ImGui::BeginChild("Fast Random Settings", ImVec2(0, 128), true);
			ImGui::Checkbox("Use Clock Time", &m_Renderer.GetSettings().UseClockTime);
			ImGui::Checkbox("Use Frame Index", &m_Renderer.GetSettings().UseFrameIndex);
			ImGui::Checkbox("Use Ray Bounces", &m_Renderer.GetSettings().UseRayBounces);
			ImGui::EndChild();
		}

		ImGui::BeginChild("Slider Settings", ImVec2(0, 90), true);
		ImGui::DragInt("Ray Bounces", &m_Renderer.GetSettings().RayBounces, 1, 2, std::numeric_limits<int>::max());
		ImGui::SliderInt("Resolution Scale", &m_Renderer.GetSettings().ResolutionScale, 1, 100, "%d%%", ImGuiSliderFlags_AlwaysClamp);
		ImGui::EndChild();

		if (ImGui::Button("Reset Accumulation", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			resetFrameIndex = true;
		}

		ImGui::End();

		SaveRendererSettings();
	}

	return resetFrameIndex;
}

void SettingsPanel::LoadRendererSettings() {
	RendererSettingsSerializer serializer(m_Renderer);
	serializer.Deserialize("settings/Renderer.yaml");
}

void SettingsPanel::SaveRendererSettings() {
	RendererSettingsSerializer serializer(m_Renderer);
	serializer.Serialize("settings/Renderer.yaml");
}

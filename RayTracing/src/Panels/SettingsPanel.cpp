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

		ImGui::Separator();
		ImGui::AlignTextToFramePadding();
		Walnut::UI::TextCentered("Renderer Settings");
		ImGui::Separator();

		ImGui::BeginChild("Boolean Settings", ImVec2(0, 128), true);
		ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);
		ImGui::Checkbox("Multithreading", &m_Renderer.GetSettings().Multithreading);
		ImGui::Checkbox("Fast Random", &m_Renderer.GetSettings().FastRandom);
		ImGui::EndChild();

		ImGui::BeginChild("Slider Settings", ImVec2(0, 90), true);
		ImGui::DragInt("Ray Bounces", &m_Renderer.GetSettings().RayBounces, 1, 2, std::numeric_limits<int>::max());
		ImGui::SliderInt("Resolution Scale", &m_Renderer.GetSettings().ResolutionScale, 1, 100, "%d%%", ImGuiSliderFlags_AlwaysClamp);
		ImGui::EndChild();

		if (m_Renderer.GetSettings().FastRandom) {
			ImGui::Separator();
			ImGui::AlignTextToFramePadding();
			Walnut::UI::TextCentered("Fast Random Settings");
			ImGui::Separator();

			ImGui::BeginChild("Fast Random Settings", ImVec2(0, 128), true);
			ImGui::Checkbox("Use Clock Time", &m_Renderer.GetSettings().UseClockTime);
			ImGui::Checkbox("Use Frame Index", &m_Renderer.GetSettings().UseFrameIndex);
			ImGui::Checkbox("Use Ray Bounces", &m_Renderer.GetSettings().UseRayBounces);
			ImGui::EndChild();
		}

		if (ImGui::Button("Reset Accumulation", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
			resetFrameIndex = true;
		}

		ImGui::Separator();
		ImGui::AlignTextToFramePadding();
		Walnut::UI::TextCentered("Language");
		ImGui::Separator();

		ImGui::BeginChild("Language Settings", ImVec2(0, 52), true);
		if (ImGui::BeginCombo("Language", m_Language.c_str())) {
			SetupLanguageSelector();
			ImGui::EndCombo();
		}
		ImGui::EndChild();

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

void SettingsPanel::SetupLanguageSelector() {
	for (const auto& entry : std::filesystem::directory_iterator("i18n")) {
		if (entry.path().extension() == ".yaml") {
			std::string languageName = entry.path().filename().replace_extension("").string();
			bool isSelected = m_Language == languageName;

			if (ImGui::Selectable(languageName.c_str(), isSelected)) {
				m_Language = languageName;
				TranslationService::Use(m_Language);
			}

			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
	}
}

#include "AboutModal.h"

#include "Walnut/Application.h"

AboutModal::AboutModal(bool& isOpen)
	: m_IsOpen(isOpen)
{}

void AboutModal::OnUIRender() {
	if (!m_IsOpen) {
		return;
	}

	ImGui::OpenPopup("About");
	m_IsOpen = ImGui::BeginPopupModal("About", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	if (m_IsOpen) {
		auto image = Walnut::Application::Get().GetApplicationIcon();
		ImGui::Image(image->GetDescriptorSet(), { 48, 48 });

		ImGui::SameLine();
		Walnut::UI::ShiftCursorX(20.0f);

		ImGui::BeginGroup();
		ImGui::Text("Ray Tracing application");
		ImGui::Text("by Gonzalo Chaparro.");
		ImGui::EndGroup();

		Walnut::UI::ShiftCursorY(20.0f);

		if (Walnut::UI::ButtonCentered("Close")) {
			m_IsOpen = false;
			ImGui::CloseCurrentPopup();
		}
	}

	ImGui::EndPopup();
}

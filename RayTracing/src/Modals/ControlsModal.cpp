#include "ControlsModal.h"

ControlsModal::ControlsModal(bool& isOpen)
	: m_IsOpen(isOpen)
{}

void ControlsModal::OnUIRender() {
	if (!m_IsOpen) {
		return;
	}

	ImGui::OpenPopup("Controls");
	m_IsOpen = ImGui::BeginPopupModal("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	if (m_IsOpen) {
		ImGui::BeginGroup();
		ImGui::Text("Left click on the Viewport to focus it.");
		ImGui::Text("While the Viewport is focused, hold right click on it to enter First Person Camera mode.");
		ImGui::Text("While in First Person Camera mode, use WASD to move around, Left Control to go down and Space to go up.");
		ImGui::Text("You can speed the camera movement up by pressing Left Shift while in First Person Camera mode.");
		ImGui::Text("You can modify the default normal and fast camera speed on the Camera section of the Scene Panel.");
		ImGui::EndGroup();

		Walnut::UI::ShiftCursorY(20.0f);

		if (Walnut::UI::ButtonCentered("Close")) {
			m_IsOpen = false;
			ImGui::CloseCurrentPopup();
		}
	}

	ImGui::EndPopup();
}

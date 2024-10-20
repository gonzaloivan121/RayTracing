#include "ViewportPanel.h"

#include "Walnut/UI/UI.h"

ViewportPanel::ViewportPanel(Renderer& renderer, Camera& camera, bool& showViewportPanel)
	: m_Renderer(renderer), m_Camera(camera), m_ShowViewportPanel(showViewportPanel)
{}

bool ViewportPanel::OnUIRender() {
	if (m_ShowViewportPanel) {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		m_ViewportFocused = ImGui::IsWindowFocused();

		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;

		auto image = m_Renderer.GetFinalImage();
		if (image) {
			ImGui::Image(image->GetDescriptorSet(), {
				(float)m_ViewportWidth,
				(float)m_ViewportHeight
				},
				{ 0.0f, 1.0f },
				{ 1.0f, 0.0f });
		}

		m_ViewportWidth *= (float)m_Renderer.GetSettings().ResolutionScale * 0.01f;
		m_ViewportHeight *= (float)m_Renderer.GetSettings().ResolutionScale * 0.01f;
		ImGui::End();
		ImGui::PopStyleVar();
	}

	return true;
}

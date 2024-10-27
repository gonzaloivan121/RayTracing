#include "StatsPanel.h"

#include "Walnut/UI/UI.h"

StatsPanel::StatsPanel(Renderer& renderer, float& lastRenderTime, bool& showStatsPanel)
	: m_Renderer(renderer), m_LastRenderTime(lastRenderTime), m_ShowStatsPanel(showStatsPanel)
{}

bool StatsPanel::OnUIRender() {
	if (m_ShowStatsPanel) {
		ImGui::Begin("Stats", &m_ShowStatsPanel);

		ImGui::BeginChild("Stats", ImVec2(0, 0), true);
		ImGui::Text("Last render: %.3fms", m_LastRenderTime);
		ImGui::Text("FPS: %.0f", ImGui::GetIO().Framerate);
		ImGui::Text("Accumulated frames: %i", m_Renderer.GetFrameIndex());
		ImGui::EndChild();

		ImGui::End();
	}

	return true;
}

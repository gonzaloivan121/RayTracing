#pragma once

#include "Panel.h"

#include "../Renderer/Renderer.h"

class StatsPanel : public Panel {
public:
	StatsPanel(Renderer& renderer, float& lastRenderTime, bool& showStatsPanel);

	virtual bool OnUIRender() override;
private:
	Renderer& m_Renderer;

	bool& m_ShowStatsPanel;

	float& m_LastRenderTime;
};
#pragma once

#include "Panel.h"

#include "../Renderer/Renderer.h"

class SettingsPanel : public Panel {
public:
	SettingsPanel(Renderer& renderer, bool& showSettingsPanel);

	virtual bool OnUIRender() override;
private:
	Renderer& m_Renderer;

	bool& m_ShowSettingsPanel;
};

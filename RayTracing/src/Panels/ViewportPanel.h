#pragma once

#include "Panel.h"

#include "../Renderer/Renderer.h"

class ViewportPanel : public Panel {
public:
	ViewportPanel(Renderer& renderer, bool& showViewportPanel);

	virtual bool OnUIRender() override;
public:
	const uint32_t& GetViewportWidth() const { return m_ViewportWidth; }
	const uint32_t& GetViewportHeight() const { return m_ViewportHeight; }

	const bool& GetViewportFocused() const { return m_ViewportFocused; }
private:
	Renderer& m_Renderer;

	uint32_t m_ViewportWidth = 0;
	uint32_t m_ViewportHeight = 0;

	bool& m_ShowViewportPanel;

	bool m_ViewportFocused = false;
};
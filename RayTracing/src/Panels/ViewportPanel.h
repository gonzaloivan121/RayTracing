#pragma once

#include "Panel.h"

#include "../Renderer/Renderer.h"
#include "../Scene/Camera.h"

class ViewportPanel : public Panel {
public:
	ViewportPanel(Renderer& renderer, Camera& camera, bool& showViewportPanel);

	const uint32_t& GetViewportWidth() const { return m_ViewportWidth; }
	const uint32_t& GetViewportHeight() const { return m_ViewportHeight; }

	virtual bool OnUIRender() override;
private:
	Renderer& m_Renderer;
	Camera& m_Camera;

	uint32_t m_ViewportWidth = 0;
	uint32_t m_ViewportHeight = 0;

	bool& m_ShowViewportPanel;

	bool m_ViewportFocused = false;
};
#pragma once

#include "Panel.h"

#include "../Renderer/Renderer.h"

#include "../Translation/TranslationService.h"

class SettingsPanel : public Panel {
public:
	SettingsPanel(Renderer& renderer, bool& showSettingsPanel);

	virtual bool OnUIRender() override;
private:
	void LoadRendererSettings();
	void SaveRendererSettings();

	void SetupLanguageSelector();
private:
	Renderer& m_Renderer;

	bool& m_ShowSettingsPanel;

	std::string m_Language = TranslationService::GetCurrentLanguage();
};

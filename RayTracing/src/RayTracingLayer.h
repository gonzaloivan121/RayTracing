#pragma once

#include "Walnut/Layer.h"

#include "Renderer/Renderer.h"

#include "Panels/StatsPanel.h"
#include "Panels/SettingsPanel.h"
#include "Panels/ScenePanel.h"
#include "Panels/ViewportPanel.h"

#include "Modals/AboutModal.h"
#include "Modals/ControlsModal.h"

class RayTracingLayer : public Walnut::Layer {
public:
	RayTracingLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(float ts) override;
	virtual void OnUIRender() override;
public:
	void NewScene(std::string& sceneName);
	void SaveScene();
	void LoadScene(std::string& sceneName);
	void LoadDefaultScene();
public:
	std::vector<std::string> GetAllScenes();
	void ExportImage();
public:
	void ShowNewSceneModal() { m_NewSceneModalOpen = true; }
	void ShowAboutModal() { m_AboutModalOpen = true; }
	void ShowControlsModal() { m_ControlsModalOpen = true; }
	void ShowCloseConfirmationModal() { m_CloseConfirmationModalOpen = true; }

	const bool& IsScenePanelShown() const { return m_ShowScenePanel; }
	const bool& IsSettingsPanelShown() const { return m_ShowSettingsPanel; }
	const bool& IsStatsPanelShown() const { return m_ShowStatsPanel; }
	const bool& IsViewportPanelShown() const { return m_ShowViewportPanel; }

	void ToggleScenePanel() { m_ShowScenePanel = !m_ShowScenePanel; }
	void ToggleSettingsPanel() { m_ShowSettingsPanel = !m_ShowSettingsPanel; }
	void ToggleStatsPanel() { m_ShowStatsPanel = !m_ShowStatsPanel; }
	void ToggleViewportPanel() { m_ShowViewportPanel = !m_ShowViewportPanel; }
public:
	const bool& AreThereUnsavedChanges() const { return m_ScenePanel.GetUnsavedChanges(); }
private:
	void UI_DrawNewSceneModal();
	void UI_DrawCloseConfirmationModal();
private:
	void Render();
	void ResetFrameIndex() { m_Renderer.ResetFrameIndex(); }
private:
	Renderer m_Renderer;
	Camera m_Camera;
	Scene m_Scene;
	Scene m_LoadedScene;

	StatsPanel m_StatsPanel;
	SettingsPanel m_SettingsPanel;
	ScenePanel m_ScenePanel;
	ViewportPanel m_ViewportPanel;

	AboutModal m_AboutModal;
	ControlsModal m_ControlsModal;

	float m_LastRenderTime = 0.0f;

	bool m_AboutModalOpen = false;
	bool m_NewSceneModalOpen = false;
	bool m_ControlsModalOpen = false;
	bool m_CloseConfirmationModalOpen = false;

	bool m_ShowStatsPanel = true;
	bool m_ShowSettingsPanel = true;
	bool m_ShowScenePanel = true;
	bool m_ShowViewportPanel = true;
};
#pragma once

#include "Panel.h"

#include "../Scene/Scene.h"
#include "../Scene/Camera.h"

class ScenePanel : public Panel {
public:
	ScenePanel(Camera& camera, Scene& scene, Scene& loadedScene, bool& showScenePanel);

	virtual bool OnUIRender() override;

	const bool& GetUnsavedChanges() const { return m_UnsavedChanges; }
private:
	bool CheckForChanges();

	void AddLight();
	void AddSphere();
	void AddMaterial();

	void RemoveLight(size_t& index);
	void RemoveSphere(size_t& index);
	void RemoveMaterial(size_t& index);
private:
	Scene& m_Scene;
	Scene& m_LoadedScene;
	Camera& m_Camera;

	bool& m_ShowScenePanel;

	bool m_UnsavedChanges = false;
};
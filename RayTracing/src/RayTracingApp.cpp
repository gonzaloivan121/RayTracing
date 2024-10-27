#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Input/Input.h"
#include "Walnut/Image.h"
#include "Walnut/UI/UI.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

#include "Renderer/Renderer.h"
#include "Scene/Camera.h"
#include "Utils/Color.h"
#include "Scene/Serializer/SceneSerializer.h"

#include "Panels/StatsPanel.h"
#include "Panels/SettingsPanel.h"
#include "Panels/ScenePanel.h"
#include "Panels/ViewportPanel.h"

#include <glm/gtc/type_ptr.hpp>

class RayTracingLayer : public Walnut::Layer {
public:
	RayTracingLayer()
		: m_Camera(45.0f, 0.1f, 1000.0f),
		m_StatsPanel(m_Renderer, m_LastRenderTime, m_ShowStatsPanel),
		m_SettingsPanel(m_Renderer, m_ShowSettingsPanel),
		m_ScenePanel(m_Camera, m_Scene, m_LoadedScene, m_ShowScenePanel),
		m_ViewportPanel(m_Renderer, m_ShowViewportPanel)
	{
		LoadDefaultScene();
	}

	virtual void OnUpdate(float ts) override {
		if (m_Camera.OnUpdate(ts, m_ViewportPanel.GetViewportFocused())) {
			ResetFrameIndex();
		}

		m_Renderer.SetTime(ts);

		if (Walnut::Input::IsKeyDown(Walnut::KeyCode::LeftControl)) {
			if (Walnut::Input::IsKeyDown(Walnut::KeyCode::N)) {
				ShowNewSceneModal();
			}

			if (Walnut::Input::IsKeyDown(Walnut::KeyCode::S)) {
				SaveScene();
			}
		}
	}

	virtual void OnUIRender() override {
		// Stats Panel
		m_StatsPanel.OnUIRender();

		// Settings Panel
		if (m_SettingsPanel.OnUIRender()) {
			ResetFrameIndex();
		}

		// Scene Panel
		if (m_ScenePanel.OnUIRender()) {
			ResetFrameIndex();
		}

		m_ViewportPanel.OnUIRender();

		UI_DrawAboutModal();
		UI_NewSceneModal();
		UI_ControlsModal();
		UI_CloseConfirmationModal();

		Render();
	}

	void UI_DrawAboutModal() {
		if (!m_AboutModalOpen) {
			return;
		}

		ImGui::OpenPopup("About");
		m_AboutModalOpen = ImGui::BeginPopupModal("About", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		if (m_AboutModalOpen) {
			auto image = Walnut::Application::Get().GetApplicationIcon();
			ImGui::Image(image->GetDescriptorSet(), { 48, 48 });

			ImGui::SameLine();
			Walnut::UI::ShiftCursorX(20.0f);

			ImGui::BeginGroup();
			ImGui::Text("Ray Tracing application");
			ImGui::Text("by Gonzalo Chaparro.");
			ImGui::EndGroup();

			Walnut::UI::ShiftCursorY(20.0f);

			if (Walnut::UI::ButtonCentered("Close")) {
				m_AboutModalOpen = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	void UI_NewSceneModal() {
		if (!m_NewSceneModalOpen) {
			return;
		}

		ImGui::OpenPopup("New Scene");
		m_NewSceneModalOpen = ImGui::BeginPopupModal("New Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		if (m_NewSceneModalOpen) {
			static char sceneName[128] = "";
			ImGui::InputText("Name", sceneName, IM_ARRAYSIZE(sceneName));
			bool disabled = strlen(sceneName) == 0;

			if (disabled) {
				ImGui::BeginDisabled();
			}

			if (ImGui::Button("Create", ImVec2(200, 0))) {
				m_NewSceneModalOpen = false;
				ImGui::CloseCurrentPopup();
				NewScene(std::string(sceneName));
				memset(sceneName, 0, IM_ARRAYSIZE(sceneName));
			}

			if (disabled) {
				ImGui::EndDisabled();
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel", ImVec2(200, 0))) {
				m_NewSceneModalOpen = false;
				ImGui::CloseCurrentPopup();
				memset(sceneName, 0, IM_ARRAYSIZE(sceneName));
			}
		}

		ImGui::EndPopup();
	}

	void UI_ControlsModal() {
		if (!m_ControlsModalOpen) {
			return;
		}

		ImGui::OpenPopup("Controls");
		m_ControlsModalOpen = ImGui::BeginPopupModal("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		if (m_ControlsModalOpen) {
			ImGui::BeginGroup();
			ImGui::Text("Left click on the Viewport to focus it.");
			ImGui::Text("While the Viewport is focused, hold right click on it to enter First Person Camera mode.");
			ImGui::Text("While in First Person Camera mode, use WASD to move around, Left Control to go down and Space to go up.");
			ImGui::Text("You can speed the camera movement up by pressing Left Shift while in First Person Camera mode.");
			ImGui::Text("You can modify the default normal and fast camera speed on the Camera section of the Scene Panel.");
			ImGui::EndGroup();

			Walnut::UI::ShiftCursorY(20.0f);

			if (Walnut::UI::ButtonCentered("Close")) {
				m_ControlsModalOpen = false;
				ImGui::CloseCurrentPopup();
			}
		}

		ImGui::EndPopup();
	}

	void UI_CloseConfirmationModal() {
		if (!m_CloseConfirmationModalOpen) {
			return;
		}

		ImGui::OpenPopup("Close Confirmation");
		m_CloseConfirmationModalOpen = ImGui::BeginPopupModal("Close Confirmation", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		if (m_CloseConfirmationModalOpen) {
			ImGui::BeginGroup();
			ImGui::Text("There are unsaved changes in this scene.");
			ImGui::Text("What do you want to do?");
			ImGui::EndGroup();

			Walnut::UI::ShiftCursorY(20.0f);

			if (ImGui::Button("Cancel")) {
				m_CloseConfirmationModalOpen = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Save and Exit")) {
				SaveScene();
				Walnut::Application::Get().Close();
			}

			ImGui::SameLine();

			if (ImGui::Button("Exit Without Saving")) {
				Walnut::Application::Get().Close();
			}
		}

		ImGui::EndPopup();
	}

	void ShowNewSceneModal() {
		m_NewSceneModalOpen = true;
	}

	void ShowAboutModal() {
		m_AboutModalOpen = true;
	}

	void ShowControlsModal() {
		m_ControlsModalOpen = true;
	}

	void ShowCloseConfirmationModal() {
		m_CloseConfirmationModalOpen = true;
	}

	void Render() {
		Walnut::Timer timer;

		m_Renderer.OnResize(m_ViewportPanel.GetViewportWidth(), m_ViewportPanel.GetViewportHeight());
		m_Camera.OnResize(m_ViewportPanel.GetViewportWidth(), m_ViewportPanel.GetViewportHeight());

		m_Renderer.Render(m_Scene, m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}

	void NewScene(std::string& sceneName) {
		m_Scene = Scene();
		m_Scene.Name = sceneName;
		Walnut::Application::Get().SetWindowTitle("Ray Tracing - " + m_Scene.Name);
		ResetFrameIndex();
	}

	void SaveScene() {
		SceneSerializer serializer(m_Scene);
		serializer.Serialize("scenes/" + m_Scene.Name + ".yaml");
		m_LoadedScene = m_Scene;
	}

	void LoadScene(std::string& sceneName) {
		SceneSerializer serializer(m_Scene);
		serializer.Deserialize("scenes/" + sceneName + ".yaml");
		m_LoadedScene = m_Scene;
		Walnut::Application::Get().SetWindowTitle("Ray Tracing - " + m_Scene.Name);
		ResetFrameIndex();
	}

	void LoadDefaultScene() {
		SceneSerializer serializer(m_Scene);
		serializer.Deserialize("scenes/Default.yaml");
		m_LoadedScene = m_Scene;
		Walnut::Application::Get().SetWindowTitle("Ray Tracing - " + m_Scene.Name);
		ResetFrameIndex();
	}

	void ResetFrameIndex() {
		m_Renderer.ResetFrameIndex();
	}

	std::vector<std::string> GetAllScenes() {
		std::vector<std::string> allScenes;

		for (const auto& entry : std::filesystem::directory_iterator("scenes")) {
			if (entry.path().extension() == ".yaml") {
				std::string sceneName = entry.path().filename().replace_extension("").string();
				if (sceneName != "Default") {
					allScenes.push_back(sceneName);
				}
			}
		}

		return allScenes;
	}

	void ExportImage() {
		std::filesystem::path folderPath = "export/" + m_Scene.Name + "/";

		if (!std::filesystem::exists(folderPath)) {
			std::filesystem::create_directory(folderPath);
			m_Renderer.GetFinalImage()->Export(m_Renderer.GetImageData(), folderPath.string() + "0.png");

			return;
		}

		int fileCount = 0;
		for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
			if (entry.path().extension() == ".png") {
				fileCount++;
			}
		}

		m_Renderer.GetFinalImage()->Export(m_Renderer.GetImageData(), folderPath.string() + std::to_string(fileCount) + ".png");
	}

	const bool& IsScenePanelShown() const { return m_ShowScenePanel; }
	const bool& IsSettingsPanelShown() const { return m_ShowSettingsPanel; }
	const bool& IsStatsPanelShown() const { return m_ShowStatsPanel; }
	const bool& IsViewportPanelShown() const { return m_ShowViewportPanel; }

	void ToggleScenePanel() { m_ShowScenePanel = !m_ShowScenePanel; }
	void ToggleSettingsPanel() { m_ShowSettingsPanel = !m_ShowSettingsPanel; }
	void ToggleStatsPanel() { m_ShowStatsPanel = !m_ShowStatsPanel; }
	void ToggleViewportPanel() { m_ShowViewportPanel = !m_ShowViewportPanel; }

	const bool& AreThereUnsavedChanges() const { return m_ScenePanel.GetUnsavedChanges(); }

private:
	Renderer m_Renderer;
	Camera m_Camera;
	Scene m_Scene;
	Scene m_LoadedScene;

	StatsPanel m_StatsPanel;
	SettingsPanel m_SettingsPanel;
	ScenePanel m_ScenePanel;
	ViewportPanel m_ViewportPanel;

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

Walnut::Application* Walnut::CreateApplication(int argc, char** argv) {
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";
	// https://tomeko.net/online_tools/file_to_hex.php?lang=en
	spec.IconPath = "resources/logo.png";
	spec.CustomTitlebar = true;
	spec.CenterWindow = true;

	Walnut::Application* app = new Walnut::Application(spec);
	std::shared_ptr<RayTracingLayer> rayTracingLayer = std::make_shared<RayTracingLayer>();
	app->PushLayer(rayTracingLayer);
	app->SetMenubarCallback([app, rayTracingLayer]() {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New Scene", "Ctrl+N")) {
				rayTracingLayer->ShowNewSceneModal();
			}
			if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
				rayTracingLayer->SaveScene();
			}
			if (ImGui::BeginMenu("Load Scene")) {
				if (ImGui::MenuItem("Default Scene")) {
					rayTracingLayer->LoadDefaultScene();
				}

				ImGui::Separator();

				auto& allScenes = rayTracingLayer->GetAllScenes();

				for (auto& scene : allScenes) {
					if (ImGui::MenuItem(scene.c_str())) {
						rayTracingLayer->LoadScene(scene);
					}
				}

				ImGui::EndMenu();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit")) {
				if (!rayTracingLayer->AreThereUnsavedChanges()) {
					app->Close();
				} else {
					rayTracingLayer->ShowCloseConfirmationModal();
				}
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View")) {
			if (ImGui::MenuItem("Show Scene", NULL, rayTracingLayer->IsScenePanelShown())) {
				rayTracingLayer->ToggleScenePanel();
			}

			if (ImGui::MenuItem("Show Settings", NULL, rayTracingLayer->IsSettingsPanelShown())) {
				rayTracingLayer->ToggleSettingsPanel();
			}

			if (ImGui::MenuItem("Show Stats", NULL, rayTracingLayer->IsStatsPanelShown())) {
				rayTracingLayer->ToggleStatsPanel();
			}

			if (ImGui::MenuItem("Show Viewport", NULL, rayTracingLayer->IsViewportPanelShown())) {
				rayTracingLayer->ToggleViewportPanel();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Export")) {
			if (ImGui::MenuItem("Image")) {
				rayTracingLayer->ExportImage();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help")) {
			if (ImGui::MenuItem("About")) {
				rayTracingLayer->ShowAboutModal();
			}

			if (ImGui::MenuItem("Controls")) {
				rayTracingLayer->ShowControlsModal();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}
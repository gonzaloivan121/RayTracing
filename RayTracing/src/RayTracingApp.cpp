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
#include "Scene/SceneSerializer.h"

#include "Panels/StatsPanel.h"
#include "Panels/SettingsPanel.h"
#include "Panels/ScenePanel.h"

#include <glm/gtc/type_ptr.hpp>

class RayTracingLayer : public Walnut::Layer {
public:
	RayTracingLayer()
		: m_Camera(45.0f, 0.1f, 1000.0f),
		m_StatsPanel(m_Renderer, m_LastRenderTime, m_ShowStatsPanel),
		m_SettingsPanel(m_Renderer, m_ShowSettingsPanel),
		m_ScenePanel(m_Camera, m_Scene, m_LoadedScene, m_ShowScenePanel)
	{
		LoadDefaultScene();
	}

	virtual void OnUpdate(float ts) override {
		if (m_Camera.OnUpdate(ts, m_ViewportFocused)) {
			ResetFrameIndex();
		}

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

		UI_DrawAboutModal();
		UI_NewSceneModal();
		UI_ControlsModal();

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
			ImGui::Text("While the Viewport is focused, right click on it to enter First Person Camera mode.");
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

	void ShowNewSceneModal() {
		m_NewSceneModalOpen = true;
	}

	void ShowAboutModal() {
		m_AboutModalOpen = true;
	}

	void ShowControlsModal() {
		m_ControlsModalOpen = true;
	}

	void Render() {
		Walnut::Timer timer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);

		m_Renderer.Render(m_Scene, m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}

	void NewScene(std::string& sceneName) {
		delete &m_LoadedScene;
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

	bool IsScenePanelShown() { return m_ShowScenePanel; }
	void ToggleScenePanel() { m_ShowScenePanel = !m_ShowScenePanel; }

	bool IsSettingsPanelShown() { return m_ShowSettingsPanel; }
	void ToggleSettingsPanel() { m_ShowSettingsPanel = !m_ShowSettingsPanel; }

	bool IsStatsPanelShown() { return m_ShowStatsPanel; }
	void ToggleStatsPanel() { m_ShowStatsPanel = !m_ShowStatsPanel; }

private:
	Renderer m_Renderer;
	Camera m_Camera;
	Scene m_Scene;
	Scene m_LoadedScene;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	StatsPanel m_StatsPanel;
	SettingsPanel m_SettingsPanel;
	ScenePanel m_ScenePanel;

	float m_LastRenderTime = 0.0f;

	bool m_AboutModalOpen = false;
	bool m_NewSceneModalOpen = false;
	bool m_ControlsModalOpen = false;
	bool m_ViewportFocused = false;

	bool m_ShowStatsPanel = true;
	bool m_ShowSettingsPanel = true;
	bool m_ShowScenePanel = true;
	bool m_ShowViewportPanel = true;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv) {
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";
	// https://tomeko.net/online_tools/file_to_hex.php?lang=en
	spec.IconPath = "res/logo.png";
	spec.CustomTitlebar = true;
	spec.CenterWindow = true;
	spec.IconPath = "";

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
				app->Close();
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
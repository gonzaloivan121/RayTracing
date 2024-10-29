#include "RayTracingLayer.h"

#include "Walnut/Application.h"
#include "Walnut/Input/Input.h"
#include "Walnut/Timer.h"
#include "Walnut/Core/Log.h"
#include "Walnut/UI/UI.h"

#include "Scene/Serializer/SceneSerializer.h"

#include "Translation/TranslationService.h"

RayTracingLayer::RayTracingLayer() :
	m_Camera(45.0f, 0.1f, 1000.0f),
	m_StatsPanel(m_Renderer, m_LastRenderTime, m_ShowStatsPanel),
	m_SettingsPanel(m_Renderer, m_ShowSettingsPanel),
	m_ScenePanel(m_Camera, m_Scene, m_LoadedScene, m_ShowScenePanel),
	m_ViewportPanel(m_Renderer, m_ShowViewportPanel),
	m_AboutModal(m_AboutModalOpen),
	m_ControlsModal(m_ControlsModalOpen)
{}

void RayTracingLayer::OnAttach() {
	spdlog::info("RayTracingLayer - Initializing");
	LoadDefaultScene();
	TranslationService::Use("English");
	spdlog::info("RayTracingLayer - Initialization complete");

}

void RayTracingLayer::OnDetach() {
	spdlog::info("RayTracingLayer - Destroying");
}

void RayTracingLayer::OnUpdate(float ts) {
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

void RayTracingLayer::OnUIRender() {
	// Panels
	m_StatsPanel.OnUIRender();
	if (m_SettingsPanel.OnUIRender()) {
		ResetFrameIndex();
	}
	if (m_ScenePanel.OnUIRender()) {
		ResetFrameIndex();
	}
	m_ViewportPanel.OnUIRender();

	// Modals
	m_AboutModal.OnUIRender();
	m_ControlsModal.OnUIRender();
	UI_DrawNewSceneModal();
	UI_DrawCloseConfirmationModal();

	// Rendering
	Render();
}

void RayTracingLayer::UI_DrawNewSceneModal() {
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

void RayTracingLayer::UI_DrawCloseConfirmationModal() {
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

void RayTracingLayer::Render() {
	Walnut::Timer timer;

	m_Renderer.OnResize(m_ViewportPanel.GetViewportWidth(), m_ViewportPanel.GetViewportHeight());
	m_Camera.OnResize(m_ViewportPanel.GetViewportWidth(), m_ViewportPanel.GetViewportHeight());

	m_Renderer.Render(m_Scene, m_Camera);

	m_LastRenderTime = timer.ElapsedMillis();
}

void RayTracingLayer::NewScene(std::string& sceneName) {
	m_Scene = Scene();
	m_Scene.Name = sceneName;
	Walnut::Application::Get().SetWindowTitle("Ray Tracing - " + m_Scene.Name);
	ResetFrameIndex();
}

void RayTracingLayer::SaveScene() {
	SceneSerializer serializer(m_Scene);
	serializer.Serialize("scenes/" + m_Scene.Name + ".yaml");
	m_LoadedScene = m_Scene;
}

void RayTracingLayer::LoadScene(std::string& sceneName) {
	SceneSerializer serializer(m_Scene);
	serializer.Deserialize("scenes/" + sceneName + ".yaml");
	m_LoadedScene = m_Scene;
	Walnut::Application::Get().SetWindowTitle("Ray Tracing - " + m_Scene.Name);
	ResetFrameIndex();
}

void RayTracingLayer::LoadDefaultScene() {
	SceneSerializer serializer(m_Scene);
	serializer.Deserialize("scenes/Default.yaml");
	m_LoadedScene = m_Scene;
	Walnut::Application::Get().SetWindowTitle("Ray Tracing - " + m_Scene.Name);
	ResetFrameIndex();
}

std::vector<std::string> RayTracingLayer::GetAllScenes() {
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

void RayTracingLayer::ExportImage() {
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
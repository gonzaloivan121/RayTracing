#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/UI/UI.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

#include "Renderer/Renderer.h"
#include "Scene/Camera.h"
#include "Utils/Color.h"
#include "Scene/SceneSerializer.h"

#include <glm/gtc/type_ptr.hpp>

class RayTracingLayer : public Walnut::Layer {
public:
	RayTracingLayer() : m_Camera(45.0f, 0.1f, 1000.0f) {
		LoadDefaultScene();
	}

	virtual void OnUpdate(float ts) override {
		if (m_Camera.OnUpdate(ts, m_ViewportFocused)) {
			ResetFrameIndex();
		}

		m_UnsavedChanges = CheckForChanges();
	}

	bool CheckForChanges() {
		if (m_LoadedScene.Name != m_Scene.Name) {
			return true;
		}

		if (!(m_LoadedScene.Sky == m_Scene.Sky)) {
			return true;
		}

		if (m_LoadedScene.Lights.size() != m_Scene.Lights.size()) {
			return true;
		}

		for (size_t i; i < m_LoadedScene.Lights.size(); i++) {
			if (!(m_LoadedScene.Lights[i] == m_Scene.Lights[i])) {
				return true;
			}
		}

		if (m_LoadedScene.Spheres.size() != m_Scene.Spheres.size()) {
			return true;
		}

		for (size_t i; i < m_LoadedScene.Spheres.size(); i++) {
			if (!(m_LoadedScene.Spheres[i] == m_Scene.Spheres[i])) {
				return true;
			}
		}

		if (m_LoadedScene.Materials.size() != m_Scene.Materials.size()) {
			return true;
		}

		for (size_t i; i < m_LoadedScene.Materials.size(); i++) {
			if (!(m_LoadedScene.Materials[i] == m_Scene.Materials[i])) {
				return true;
			}
		}

		return false;
	}

	virtual void OnUIRender() override {
		ImGui::Begin("Stats");
		ImGui::BeginChild("Stats", ImVec2(0, 0), true);
		ImGui::Text("Last render: %.3fms", m_LastRenderTime);
		ImGui::Text("FPS: %.0f", ImGui::GetIO().Framerate);
		ImGui::Text("Accumulated frames: %i", m_Renderer.GetFrameIndex());
		ImGui::EndChild();
		ImGui::End();

		ImGui::Begin("Settings");
		ImGui::BeginChild("Settings", ImVec2(0, 166), true);
		ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);
		ImGui::Checkbox("Multithreading", &m_Renderer.GetSettings().Multithreading);
		ImGui::Checkbox("PCH Random", &m_Renderer.GetSettings().PCHRandom);
		ImGui::DragInt("Ray Bounces", &m_Renderer.GetSettings().RayBounces, 1, 1, std::numeric_limits<int>::max());
		ImGui::EndChild();

		if (ImGui::Button("Reset Accumulation")) {
			ResetFrameIndex();
		}
		ImGui::End();

		ImGuiWindowFlags windowFlags = 0;
		if (m_UnsavedChanges) {
			windowFlags |= ImGuiWindowFlags_UnsavedDocument;
		} else {
			windowFlags = 0;
		}

		ImGui::Begin("Scene", NULL, windowFlags);

		// Sky
		ImGui::Separator();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Sky");
		ImGui::Separator();

		ImGui::BeginChild("Sky##Settings", ImVec2(0, 90), true);
		Sky& sky = m_Scene.Sky;
		ImGui::Checkbox("Enabled", &sky.Enabled);
		ImGui::ColorEdit3("Color", glm::value_ptr(sky.Color));
		ImGui::EndChild();

		// Lights
		ImGui::Separator();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Lights");
		ImGui::SameLine();
		Walnut::UI::ShiftCursorX(ImGui::GetColumnWidth() - 50.0f);
		if (ImGui::Button("Add##Light")) {
			AddLight();
		}
		ImGui::Separator();

		for (size_t i = 0; i < m_Scene.Lights.size(); i++) {
			ImGui::PushID(i);

			ImGui::BeginChild("Light", ImVec2(0, 128), true);
			Light& light = m_Scene.Lights[i];
			ImGui::Checkbox("Enabled", &light.Enabled);
			ImGui::DragFloat3("Direction", glm::value_ptr(light.Direction), 0.01f);
			if (ImGui::Button("Remove", ImGui::GetContentRegionAvail())) {
				RemoveLight(i);
			}
			ImGui::EndChild();

			ImGui::PopID();
		}

		// Spheres
		ImGui::Separator();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Spheres");
		ImGui::SameLine();
		Walnut::UI::ShiftCursorX(ImGui::GetColumnWidth() - 50.0f);
		if (ImGui::Button("Add##Sphere")) {
			AddSphere();
		}
		ImGui::Separator();

		for (size_t i = 0; i < m_Scene.Spheres.size(); i++) {
			ImGui::PushID(i);

			ImGui::BeginChild("Sphere", ImVec2(0, 204), true);
			Sphere& sphere = m_Scene.Spheres[i];
			ImGui::Checkbox("Enabled", &sphere.Enabled);
			ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.01f);
			ImGui::DragFloat("Radius", &sphere.Radius, 0.01f);
			if (ImGui::BeginCombo("Material", m_Scene.Materials[sphere.MaterialIndex].Name.c_str())) {
				for (size_t j = 0; j < m_Scene.Materials.size(); j++) {
					bool isSelected = m_Scene.Materials[sphere.MaterialIndex] == m_Scene.Materials[j];
					if (ImGui::Selectable(m_Scene.Materials[j].Name.c_str(), isSelected)) {
						sphere.MaterialIndex = j;
					}
					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			if (ImGui::Button("Remove", ImGui::GetContentRegionAvail())) {
				RemoveSphere(i);
			}
			ImGui::EndChild();

			ImGui::PopID();
		}

		// Materials
		ImGui::Separator();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Materials");
		ImGui::SameLine();
		Walnut::UI::ShiftCursorX(ImGui::GetColumnWidth() - 50.0f);
		if (ImGui::Button("Add##Material")) {
			AddMaterial();
		}
		ImGui::Separator();

		for (size_t i = 0; i < m_Scene.Materials.size(); i++) {
			ImGui::PushID(i);

			ImGui::BeginChild("Material", ImVec2(0, 280), true);
			Material& material = m_Scene.Materials[i];
			ImGui::InputText("Name", material.Name.data(), sizeof(std::string) * 8);
			ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo));
			ImGui::SliderFloat("Roughness", &material.Roughness, 0.0f, 1.0f);
			ImGui::SliderFloat("Metallic", &material.Metallic, 0.0f, 1.0f);
			ImGui::ColorEdit3("Emission Color", glm::value_ptr(material.EmissionColor));
			ImGui::DragFloat("Emission Power", &material.EmissionPower, 0.01f, 0.0f, std::numeric_limits<float>::max());
			if (ImGui::Button("Remove", ImGui::GetContentRegionAvail())) {
				RemoveMaterial(i);
			}
			ImGui::EndChild();

			ImGui::PopID();
		}

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		m_ViewportFocused = ImGui::IsWindowFocused();

		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;

		auto image = m_Renderer.GetFinalImage();
		if (image) {
			ImGui::Image(image->GetDescriptorSet(), {
				(float)image->GetWidth(),
				(float)image->GetHeight()
			}, {0, 1}, {1, 0});
		}

		ImGui::End();
		ImGui::PopStyleVar();

		UI_DrawAboutModal();

		Render();
	}

	void AddLight() {
		m_Scene.Lights.emplace_back();
		ResetFrameIndex();
	}

	void RemoveLight(size_t& index) {
		m_Scene.Lights.erase(m_Scene.Lights.begin() + index);
		ResetFrameIndex();
	}

	void AddSphere() {
		m_Scene.Spheres.emplace_back();
		ResetFrameIndex();
	}

	void RemoveSphere(size_t& index) {
		m_Scene.Spheres.erase(m_Scene.Spheres.begin() + index);
		ResetFrameIndex();
	}

	void AddMaterial() {
		m_Scene.Materials.emplace_back();
		ResetFrameIndex();
	}

	void RemoveMaterial(size_t& index) {
		m_Scene.Materials.erase(m_Scene.Materials.begin() + index);
		ResetFrameIndex();
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

	void ShowAboutModal() {
		m_AboutModalOpen = true;
	}

	void Render() {
		Walnut::Timer timer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);

		m_Renderer.Render(m_Scene, m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}

	void NewScene() {
		m_Scene = Scene();
		m_LoadedScene = m_Scene;
		Walnut::Application::Get().SetWindowTitle("Ray Tracing - " + m_Scene.Name);
		ResetFrameIndex();
	}

	void SaveScene() {
		SceneSerializer serializer(m_Scene);
		serializer.Serialize("scenes/" + m_Scene.Name + ".yaml");
		m_LoadedScene = m_Scene;
	}

	void LoadScene() {
		SceneSerializer serializer(m_Scene);
		serializer.Deserialize("scenes/" + m_Scene.Name + ".yaml");
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

private:
	Renderer m_Renderer;
	Camera m_Camera;
	Scene m_Scene;
	Scene m_LoadedScene;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	float m_LastRenderTime = 0.0f;

	bool m_AboutModalOpen = false;
	bool m_ViewportFocused = false;
	bool m_UnsavedChanges = false;
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
			if (ImGui::MenuItem("New")) {
				rayTracingLayer->NewScene();
			}
			if (ImGui::MenuItem("Save")) {
				rayTracingLayer->SaveScene();
			}
			if (ImGui::MenuItem("Load")) {
				rayTracingLayer->LoadScene();
			}
			if (ImGui::MenuItem("Exit")) {
				app->Close();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help")) {
			if (ImGui::MenuItem("About")) {
				rayTracingLayer->ShowAboutModal();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}
#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/UI/UI.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

#include "Renderer.h"
#include "Camera.h"
#include "Color.h"

#include <glm/gtc/type_ptr.hpp>

class RayTracingLayer : public Walnut::Layer {
public:
	RayTracingLayer() : m_Camera(45.0f, 0.1f, 1000.0f) {
		Walnut::Application::Get().SetWindowTitle("Ray Tracing - " + m_Scene.Name);

		Material& pinkSphere = m_Scene.Materials.emplace_back();
		pinkSphere.Albedo = Color::Magenta;
		pinkSphere.Roughness = 0.0f;

		Material& blueSphere = m_Scene.Materials.emplace_back();
		blueSphere.Albedo = Color::Blue;
		blueSphere.Roughness = 0.1f;

		Material& orangeSphere = m_Scene.Materials.emplace_back();
		orangeSphere.Albedo = Color::Orange;
		orangeSphere.Roughness = 0.1f;
		orangeSphere.EmissionColor = orangeSphere.Albedo;
		orangeSphere.EmissionPower = 1000.0f;

		{
			// Magenta
			Sphere sphere;
			sphere.Position = { -1.0f, 0.0f, 0.0f };
			sphere.Radius = 1.0f;
			sphere.MaterialIndex = 0;
			m_Scene.Spheres.push_back(sphere);
		}

		{
			// Orange
			Sphere sphere;
			sphere.Position = { 3000.0f, 2000.0f, -10000.0f };
			sphere.Radius = 1000.0f;
			sphere.MaterialIndex = 2;
			m_Scene.Spheres.push_back(sphere);
		}

		{
			// Blue
			Sphere sphere;
			sphere.Position = { 0.0f, -101.0f, 0.0f };
			sphere.Radius = 100.0f;
			sphere.MaterialIndex = 1;
			m_Scene.Spheres.push_back(sphere);
		}

		Light light;
		m_Scene.Light = light;
	}

	virtual void OnUpdate(float ts) override {
		if (m_Camera.OnUpdate(ts)) {
			m_Renderer.ResetFrameIndex();
		}
	}

	virtual void OnUIRender() override {
		ImGui::Begin("Stats");
		ImGui::BeginChild("Stats", ImVec2(0, 0), true);
		ImGui::Text("Last render: %.3fms", m_LastRenderTime);
		ImGui::Text("Accumulated frames: %i", m_Renderer.GetFrameIndex());
		ImGui::EndChild();
		ImGui::End();

		ImGui::Begin("Settings");
		ImGui::BeginChild("Settings", ImVec2(0, 128), true);
		ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);
		ImGui::Checkbox("Multithreading", &m_Renderer.GetSettings().Multithreading);
		ImGui::Checkbox("PCH Random", &m_Renderer.GetSettings().PCHRandom);
		ImGui::EndChild();

		if (ImGui::Button("Reset Accumulation")) {
			m_Renderer.ResetFrameIndex();
		}
		ImGui::End();

		ImGui::Begin("Scene");

		// Light
		ImGui::Text("Light");
		ImGui::BeginChild("Light", ImVec2(0, 52), true);
		ImGui::DragFloat3("Direction", glm::value_ptr(m_Scene.Light.Direction), 0.01f);
		ImGui::EndChild();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// Spheres
		ImGui::Text("Spheres");
		ImGui::SameLine();
		Walnut::UI::ShiftCursorX(ImGui::GetColumnWidth() - 50.0f);

		if (ImGui::Button("Add##Sphere")) {
			AddSphere();
		}

		for (size_t i = 0; i < m_Scene.Spheres.size(); i++) {
			ImGui::PushID(i);

			ImGui::BeginChild("Sphere", ImVec2(0, 166), true);
			Sphere& sphere = m_Scene.Spheres[i];
			ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.01f);
			ImGui::DragFloat("Radius", &sphere.Radius, 0.01f);
			ImGui::DragInt("Material", &sphere.MaterialIndex, 1.0f, 0, (int)m_Scene.Materials.size() - 1);
			if (ImGui::Button("Remove", ImGui::GetContentRegionAvail())) {
				RemoveSphere(i);
			}
			ImGui::EndChild();

			ImGui::PopID();
		}

		ImGui::Separator();
		ImGui::Spacing();

		// Materials
		ImGui::Text("Materials");
		ImGui::SameLine();
		Walnut::UI::ShiftCursorX(ImGui::GetColumnWidth() - 50.0f);

		if (ImGui::Button("Add##Material")) {
			AddMaterial();
		}

		for (size_t i = 0; i < m_Scene.Materials.size(); i++) {
			ImGui::PushID(i);

			ImGui::BeginChild("Material", ImVec2(0, 242), true);
			Material& material = m_Scene.Materials[i];
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

		ImGui::Separator();
		ImGui::Spacing();

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

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

	void AddSphere() {
		m_Scene.Spheres.emplace_back();
	}

	void RemoveSphere(size_t& index) {
		m_Scene.Spheres.erase(m_Scene.Spheres.begin() + index);
	}

	void AddMaterial() {
		m_Scene.Materials.emplace_back();
	}

	void RemoveMaterial(size_t& index) {
		m_Scene.Materials.erase(m_Scene.Materials.begin() + index);
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

	void SaveScene() {
		
	}

private:
	Renderer m_Renderer;
	Camera m_Camera;
	Scene m_Scene;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	float m_LastRenderTime = 0.0f;

	bool m_AboutModalOpen = false;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv) {
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";
	spec.IconPath = "res/logo.png";
	spec.CustomTitlebar = true;
	spec.CenterWindow = true;
	spec.IconPath = "";

	Walnut::Application* app = new Walnut::Application(spec);
	std::shared_ptr<RayTracingLayer> rayTracingLayer = std::make_shared<RayTracingLayer>();
	app->PushLayer(rayTracingLayer);
	app->SetMenubarCallback([app, rayTracingLayer]() {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Save")) {
				rayTracingLayer->SaveScene();
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
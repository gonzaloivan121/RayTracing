#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

#include "Renderer.h"
#include "Camera.h"
#include "Color.h"

#include <glm/gtc/type_ptr.hpp>

using namespace Walnut;

class RayTracingLayer : public Walnut::Layer {
public:
	RayTracingLayer() : m_Camera(45.0f, 0.1f, 1000.0f) {

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
		ImGui::Text("Last render: %.3fms", m_LastRenderTime);
		ImGui::Text("Accumulated frames: %i", m_Renderer.GetFrameIndex());
		ImGui::End();

		ImGui::Begin("Settings");
		ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);
		ImGui::Checkbox("Multithreading", &m_Renderer.GetSettings().Multithreading);
		ImGui::Checkbox("PCH Random", &m_Renderer.GetSettings().PCHRandom);

		if (ImGui::Button("Reset Accumulation")) {
			m_Renderer.ResetFrameIndex();
		}
		ImGui::End();

		ImGui::Begin("Scene");

		// Light
		ImGui::Text("Light:");
		ImGui::DragFloat3("Direction", glm::value_ptr(m_Scene.Light.Direction), 0.01f);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// Spheres
		ImGui::Text("Spheres:");

		for (size_t i = 0; i < m_Scene.Spheres.size(); i++) {
			ImGui::PushID(i);

			Sphere& sphere = m_Scene.Spheres[i];
			ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.01f);
			ImGui::DragFloat("Radius", &sphere.Radius, 0.01f);
			ImGui::DragInt("Material", &sphere.MaterialIndex, 1.0f, 0, (int)m_Scene.Materials.size() - 1);
			ImGui::Spacing();

			ImGui::PopID();
		}

		ImGui::Separator();
		ImGui::Spacing();

		// Materials
		ImGui::Text("Materials:");

		for (size_t i = 0; i < m_Scene.Materials.size(); i++) {
			ImGui::PushID(i);

			Material& material = m_Scene.Materials[i];
			ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo));
			ImGui::SliderFloat("Rougness", &material.Roughness, 0.0f, 1.0f);
			ImGui::SliderFloat("Metallic", &material.Metallic, 0.0f, 1.0f);
			ImGui::ColorEdit3("Emission Color", glm::value_ptr(material.EmissionColor));
			ImGui::DragFloat("Emission Power", &material.EmissionPower, 0.01f, 0.0f, std::numeric_limits<float>::max());
			ImGui::Spacing();

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

		Render();
	}

	void Render() {
		Timer timer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);

		m_Renderer.Render(m_Scene, m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}

private:
	Renderer m_Renderer;
	Camera m_Camera;
	Scene m_Scene;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	float m_LastRenderTime = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv) {
	Walnut::ApplicationSpecification spec;
	spec.Name = "RayTracing";
	//spec.CustomTitleBar = true;

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<RayTracingLayer>();
	app->SetMenubarCallback([app]() {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Exit")) {
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}
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
		{
			Sphere sphere;
			sphere.Position = { 0.0f, 0.0f, 0.0f };
			sphere.Radius = 1.0f;
			sphere.Material.Albedo = Color::Magenta;
			m_Scene.Spheres.push_back(sphere);
		}

		{
			Sphere sphere;
			sphere.Position = { 0.0f, -101.0f, 0.0f };
			sphere.Radius = 100.0f;
			sphere.Material.Albedo = Color::Blue;
			m_Scene.Spheres.push_back(sphere);
		}

		Light light;
		m_Scene.Light = light;
	}

	virtual void OnUpdate(float ts) override {
		m_Camera.OnUpdate(ts);
	}

	virtual void OnUIRender() override {
		ImGui::Begin("Stats");
		ImGui::Text("Last render: %.3fms", m_LastRenderTime);
		ImGui::End();

		ImGui::Begin("Scene");
		ImGui::Text("Light:");
		ImGui::DragFloat3("Light Direction", glm::value_ptr(m_Scene.Light.Direction), 0.01f);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Text("Spheres:");

		for (size_t i = 0; i < m_Scene.Spheres.size(); i++) {
			ImGui::PushID(i);

			Sphere& sphere = m_Scene.Spheres[i];
			ImGui::DragFloat3("Sphere Position", glm::value_ptr(sphere.Position), 0.01f);
			ImGui::DragFloat("Sphere Radius", &sphere.Radius, 0.01f);
			ImGui::ColorEdit3("Sphere Albedo", glm::value_ptr(sphere.Material.Albedo));
			ImGui::SliderFloat("Sphere Rougness", &sphere.Material.Roughness, 0.0f, 1.0f);
			ImGui::SliderFloat("Sphere Metallic", &sphere.Material.Metallic, 0.0f, 1.0f);
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
	ApplicationSpecification spec;
	spec.Name = "RayTracing";

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
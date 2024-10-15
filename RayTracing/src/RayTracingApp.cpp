#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

#include "Renderer.h"
#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace Walnut;

class RayTracingLayer : public Walnut::Layer {
public:
	RayTracingLayer() : m_Camera(45.0f, 0.1f, 1000.0f) {}

	virtual void OnUpdate(float ts) override {
		m_Camera.OnUpdate(ts);
	}

	virtual void OnUIRender() override {
		ImGui::Begin("Settings");
		ImGui::Text("Last render: %.3fms", m_LastRenderTime);
		ImGui::DragFloat3("Light Direction", glm::value_ptr(m_LightDirection), 0.01f);
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

		m_Renderer.Render(m_Camera, m_LightDirection);

		m_LastRenderTime = timer.ElapsedMillis();
	}

private:
	Renderer m_Renderer;
	Camera m_Camera;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	float m_LastRenderTime = 0.0f;

	glm::vec3 m_LightDirection = glm::vec3(-1, -1, -1);
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
#include "ScenePanel.h"

#include "Walnut/UI/UI.h"

#include <glm/gtc/type_ptr.hpp>

ScenePanel::ScenePanel(Camera& camera, Scene& scene, Scene& loadedScene, bool& showScenePanel)
	: m_Camera(camera), m_Scene(scene), m_LoadedScene(loadedScene), m_ShowScenePanel(showScenePanel)
{}

bool ScenePanel::OnUIRender() {
	bool resetFrameIndex = false;

	if (m_ShowScenePanel) {
		m_UnsavedChanges = CheckForChanges();

		ImGuiWindowFlags windowFlags = 0;
		if (m_UnsavedChanges) {
			windowFlags |= ImGuiWindowFlags_UnsavedDocument;
		} else {
			windowFlags = 0;
		}

		ImGui::Begin("Scene", &m_ShowScenePanel, windowFlags);

		// Camera
		ImGui::Separator();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Camera");
		ImGui::Separator();

		ImGui::BeginChild("Camera##Settings", ImVec2(0, 280), true);
		CameraData& cameraData = m_Camera.GetCameraData();
		ImGui::DragFloat3("Position", glm::value_ptr(cameraData.Position), 0.01f);
		ImGui::DragFloat("Vertical FOV", &cameraData.VerticalFOV, 0.01f, 1.0f, 179.0f);
		ImGui::DragFloat("Near Clip", &cameraData.NearClip, 0.01f, 0.01f, std::numeric_limits<float>::max());
		ImGui::DragFloat("Far Clip", &cameraData.FarClip, 0.01f, 0.01f, std::numeric_limits<float>::max());
		ImGui::DragFloat("Normal Speed", &cameraData.NormalMovementSpeed, 0.01f, 0.0f, std::numeric_limits<float>::max());
		ImGui::DragFloat("Fast Speed", &cameraData.FastMovementSpeed, 0.01f, 0.0f, std::numeric_limits<float>::max());
		ImGui::DragFloat("Rotation Speed", &cameraData.RotationSpeed, 0.01f, 0.0f, std::numeric_limits<float>::max());
		ImGui::EndChild();

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
			resetFrameIndex = true;
		}
		ImGui::Separator();

		for (size_t i = 0; i < m_Scene.Lights.size(); i++) {
			ImGui::PushID(i);

			ImGui::BeginChild("Light", ImVec2(0, 128), true);
			Light& light = m_Scene.Lights[i];
			ImGui::Checkbox("Enabled", &light.Enabled);
			ImGui::DragFloat3("Position", glm::value_ptr(light.Position), 0.01f);
			if (ImGui::Button("Remove", ImGui::GetContentRegionAvail())) {
				RemoveLight(i);
				resetFrameIndex = true;
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
			resetFrameIndex = true;
		}
		ImGui::Separator();

		for (size_t i = 0; i < m_Scene.Spheres.size(); i++) {
			ImGui::PushID(i);

			ImGui::BeginChild("Sphere", ImVec2(0, 204), true);
			Sphere& sphere = m_Scene.Spheres[i];
			ImGui::Checkbox("Enabled", &sphere.Enabled);
			ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.01f);
			ImGui::DragFloat("Radius", &sphere.Radius, 0.01f);
			if (m_Scene.Materials.size() > 0) {
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
			}
			if (ImGui::Button("Remove", ImGui::GetContentRegionAvail())) {
				RemoveSphere(i);
				resetFrameIndex = true;
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
			resetFrameIndex = true;
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
				resetFrameIndex = true;
			}
			ImGui::EndChild();

			ImGui::PopID();
		}

		ImGui::End();
	}

	return resetFrameIndex;
}

bool ScenePanel::CheckForChanges() {
	if (m_LoadedScene.Name != m_Scene.Name) {
		return true;
	}

	if (!(m_LoadedScene.Sky == m_Scene.Sky)) {
		return true;
	}

	if (m_LoadedScene.Lights.size() != m_Scene.Lights.size()) {
		return true;
	}

	for (size_t i = 0; i < m_LoadedScene.Lights.size(); i++) {
		if (!(m_LoadedScene.Lights[i] == m_Scene.Lights[i])) {
			return true;
		}
	}

	if (m_LoadedScene.Spheres.size() != m_Scene.Spheres.size()) {
		return true;
	}

	for (size_t i = 0; i < m_LoadedScene.Spheres.size(); i++) {
		if (!(m_LoadedScene.Spheres[i] == m_Scene.Spheres[i])) {
			return true;
		}
	}

	if (m_LoadedScene.Materials.size() != m_Scene.Materials.size()) {
		return true;
	}

	for (size_t i = 0; i < m_LoadedScene.Materials.size(); i++) {
		if (!(m_LoadedScene.Materials[i] == m_Scene.Materials[i])) {
			return true;
		}
	}

	return false;
}

void ScenePanel::AddLight() {
	m_Scene.Lights.emplace_back();
}

void ScenePanel::AddSphere() {
	m_Scene.Spheres.emplace_back();
}

void ScenePanel::AddMaterial() {
	m_Scene.Materials.emplace_back();
}

void ScenePanel::RemoveLight(size_t& index) {
	m_Scene.Lights.erase(m_Scene.Lights.begin() + index);
}

void ScenePanel::RemoveSphere(size_t& index) {
	m_Scene.Spheres.erase(m_Scene.Spheres.begin() + index);
}

void ScenePanel::RemoveMaterial(size_t& index) {
	m_Scene.Materials.erase(m_Scene.Materials.begin() + index);
}

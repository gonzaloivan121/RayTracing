#pragma once

#include <glm/glm.hpp>
#include <vector>

struct CameraData {
	glm::vec3 Position = glm::vec3(0.0f, 0.0f, 6.0f);
	float VerticalFOV = 45.0f;
	float NearClip = 0.1f;
	float FarClip = 100.0f;

	float NormalMovementSpeed = 5.0f;
	float FastMovementSpeed = 10.0f;
	float RotationSpeed = 0.3f;
};

class Camera {
public:
	Camera(float verticalFOV, float nearClip, float farClip);

	bool OnUpdate(const float& ts, const bool& viewportFocused);
	void OnResize(uint32_t width, uint32_t height);

	const glm::mat4& GetProjection() const { return m_Projection; }
	const glm::mat4& GetInverseProjection() const { return m_InverseProjection; }
	const glm::mat4& GetView() const { return m_View; }
	const glm::mat4& GetInverseView() const { return m_InverseView; }

	const glm::vec3& GetPosition() const { return m_Data.Position; }
	const glm::vec3& GetDirection() const { return m_ForwardDirection; }

	const std::vector<glm::vec3>& GetRayDirections() const { return m_RayDirections; }

	CameraData& GetCameraData() { return m_Data; }
private:
	void RecalculateProjection();
	void RecalculateView();
	void RecalculateRayDirections();
private:
	CameraData m_Data;

	glm::mat4 m_Projection{ 1.0f };
	glm::mat4 m_View{ 1.0f };
	glm::mat4 m_InverseProjection{ 1.0f };
	glm::mat4 m_InverseView{ 1.0f };

	glm::vec3 m_ForwardDirection{ 0.0f, 0.0f, -1.0f };
	glm::vec3 m_UpDirection{ 0.0f, 1.0f, 0.0f };

	// Cached ray directions
	std::vector<glm::vec3> m_RayDirections;

	glm::vec2 m_LastMousePosition{ 0.0f, 0.0f };

	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
};
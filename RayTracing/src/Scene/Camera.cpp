#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Walnut/Input/Input.h"

Camera::Camera(float verticalFOV, float nearClip, float farClip) {
	m_Data.VerticalFOV = verticalFOV;
	m_Data.NearClip = nearClip;
	m_Data.FarClip = farClip;
	m_Data.Position = glm::vec3(0.0f, 0.0f, 6.0f);
}

bool Camera::OnUpdate(float ts, bool viewportFocused) {
	glm::vec2 mousePos = Walnut::Input::GetMousePosition();
	glm::vec2 delta = (mousePos - m_LastMousePosition) * 0.002f;
	m_LastMousePosition = mousePos;

	if (m_ViewportWidth != 0 && m_ViewportHeight != 0) {
		RecalculateView();
		RecalculateProjection();
		RecalculateRayDirections();
	}

	if (!Walnut::Input::IsMouseButtonDown(Walnut::MouseButton::Right)) {
		Walnut::Input::SetCursorMode(Walnut::CursorMode::Normal);
		return false;
	}

	if (!viewportFocused) {
		return false;
	}

	Walnut::Input::SetCursorMode(Walnut::CursorMode::Locked);

	bool moved = false;

	glm::vec3 rightDirection = glm::cross(m_ForwardDirection, m_UpDirection);

	float speed = m_Data.NormalMovementSpeed;

	if (Walnut::Input::IsKeyDown(Walnut::KeyCode::LeftShift)) {
		speed = m_Data.FastMovementSpeed;
	}

	// Movement
	if (Walnut::Input::IsKeyDown(Walnut::KeyCode::W)) {
		m_Data.Position += m_ForwardDirection * speed * ts;
		moved = true;
	} else if (Walnut::Input::IsKeyDown(Walnut::KeyCode::S)) {
		m_Data.Position -= m_ForwardDirection * speed * ts;
		moved = true;
	}

	if (Walnut::Input::IsKeyDown(Walnut::KeyCode::A)) {
		m_Data.Position -= rightDirection * speed * ts;
		moved = true;
	} else if (Walnut::Input::IsKeyDown(Walnut::KeyCode::D)) {
		m_Data.Position += rightDirection * speed * ts;
		moved = true;
	}

	if (Walnut::Input::IsKeyDown(Walnut::KeyCode::LeftControl)) {
		m_Data.Position -= m_UpDirection * speed * ts;
		moved = true;
	} else if (Walnut::Input::IsKeyDown(Walnut::KeyCode::Space)) {
		m_Data.Position += m_UpDirection * speed * ts;
		moved = true;
	}

	// Rotation
	if (delta.x != 0.0f || delta.y != 0.0f) {
		float pitchDelta = delta.y * m_Data.RotationSpeed;
		float yawDelta = delta.x * m_Data.RotationSpeed;

		glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, rightDirection),
			glm::angleAxis(-yawDelta, m_UpDirection)));
		m_ForwardDirection = glm::rotate(q, m_ForwardDirection);

		moved = true;
	}

	/*if (moved) {
		RecalculateView();
		RecalculateRayDirections();
	}*/

	return moved;
}

void Camera::OnResize(uint32_t width, uint32_t height) {
	if (width == m_ViewportWidth && height == m_ViewportHeight)
		return;

	m_ViewportWidth = width;
	m_ViewportHeight = height;

	RecalculateProjection();
	RecalculateRayDirections();
}

void Camera::RecalculateProjection() {
	m_Projection = glm::perspectiveFov(glm::radians(m_Data.VerticalFOV), (float)m_ViewportWidth, (float)m_ViewportHeight, m_Data.NearClip, m_Data.FarClip);
	m_InverseProjection = glm::inverse(m_Projection);
}

void Camera::RecalculateView() {
	m_View = glm::lookAt(m_Data.Position, m_Data.Position + m_ForwardDirection, m_UpDirection);
	m_InverseView = glm::inverse(m_View);
}

void Camera::RecalculateRayDirections() {
	m_RayDirections.resize(m_ViewportWidth * m_ViewportHeight);

	for (uint32_t y = 0; y < m_ViewportHeight; y++) {
		for (uint32_t x = 0; x < m_ViewportWidth; x++) {
			glm::vec2 coord = { (float)x / (float)m_ViewportWidth, (float)y / (float)m_ViewportHeight };
			coord = coord * 2.0f - 1.0f; // -1 -> 1

			glm::vec4 target = m_InverseProjection * glm::vec4(coord.x, coord.y, 1, 1);
			glm::vec3 rayDirection = glm::vec3(m_InverseView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0)); // World space
			m_RayDirections[x + y * m_ViewportWidth] = rayDirection;
		}
	}
}
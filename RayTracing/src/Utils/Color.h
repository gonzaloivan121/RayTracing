#pragma once

#include <glm/glm.hpp>

class Color {
public:
	static constexpr glm::vec3& White = glm::vec3(1.0f);
	static constexpr glm::vec3& Black = glm::vec3(0.0f, 0.0f, 0.0f);
	static constexpr glm::vec3& Red = glm::vec3(1.0f, 0.0f, 0.0f);
	static constexpr glm::vec3& Green = glm::vec3(0.0f, 1.0f, 0.0f);
	static constexpr glm::vec3& Blue = glm::vec3(0.0f, 0.0f, 1.0f);
	static constexpr glm::vec3& Magenta = glm::vec3(1.0f, 0.0f, 1.0f);
	static constexpr glm::vec3& Orange = glm::vec3(0.8f, 0.5f, 0.2f);
	static constexpr glm::vec3& Sky = glm::vec3(0.6f, 0.7f, 0.9f);
};
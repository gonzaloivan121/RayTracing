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
};
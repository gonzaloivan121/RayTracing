#pragma once

#include "Color.h"

#include <glm/glm.hpp>

#include <vector>

struct Light {
	glm::vec3 Position{ -1.0f };
};

struct Sphere {
	glm::vec3 Position{ 0.0f };
	float Radius = 0.5f;

	glm::vec3 Albedo = Color::Magenta;
};

struct Scene {
	std::vector<Sphere> Spheres;
	Light Light;
};
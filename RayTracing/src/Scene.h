#pragma once

#include "Color.h"

#include <glm/glm.hpp>

#include <vector>

struct Light {
	glm::vec3 Direction{ -1.0f };
};

struct Material {
	glm::vec3 Albedo = Color::White;
	float Roughness = 1.0f;
	float Metallic = 0.0f;
};

struct Sphere {
	glm::vec3 Position{ 0.0f };
	float Radius = 0.5f;

	Material Material;
};

struct Scene {
	std::vector<Sphere> Spheres;
	Light Light;
};
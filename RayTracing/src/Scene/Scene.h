#pragma once

#include "../Utils/Color.h"

#include <glm/glm.hpp>

#include <vector>
#include <string>

struct Light {
	glm::vec3 Direction{ -1.0f };
};

struct Material {
	glm::vec3 Albedo = Color::White;
	float Roughness = 1.0f;
	float Metallic = 0.0f;
	glm::vec3 EmissionColor = Color::Black;
	float EmissionPower = 0.0f;

	glm::vec3& GetEmission() const { return EmissionColor * EmissionPower; }
};

struct Sphere {
	glm::vec3 Position{ 0.0f };
	float Radius = 0.5f;

	int MaterialIndex = 0;
};

struct Scene {
	std::string Name = "Default";

	std::vector<Sphere> Spheres;
	std::vector<Material> Materials;
	std::vector<Light> Lights;
};
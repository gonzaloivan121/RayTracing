#pragma once

#include "../Utils/Color.h"

#include <glm/glm.hpp>

#include <vector>
#include <string>

struct Sky {
	bool Enabled = false;
	glm::vec3 Color = Color::Sky;

	bool operator==(const Sky& other) {
		return (
			this->Enabled == other.Enabled &&
			this->Color == other.Color
		);
	}
};

struct Light {
	bool Enabled = true;
	glm::vec3 Position{ 0.0f };

	bool operator==(const Light& other) {
		return (
			this->Enabled == other.Enabled &&
			this->Position == other.Position
		);
	}
};

struct Material {
	std::string Name = "Default Material";
	glm::vec3 Albedo = Color::White;
	float Roughness = 1.0f;
	float Metallic = 0.0f;
	glm::vec3 EmissionColor = Color::Black;
	float EmissionPower = 0.0f;

	glm::vec3& GetEmission() const { return EmissionColor * EmissionPower; }

	bool operator==(const Material& other) {
		return (
			this->Name == other.Name &&
			this->Albedo == other.Albedo &&
			this->Roughness == other.Roughness &&
			this->Metallic == other.Metallic &&
			this->EmissionColor == other.EmissionColor &&
			this->EmissionPower == other.EmissionPower
		);
	}
};

struct Sphere {
	bool Enabled = true;
	glm::vec3 Position{ 0.0f };
	float Radius = 0.5f;

	int MaterialIndex = 0;

	bool operator==(const Sphere& other) {
		return (
			this->Enabled == other.Enabled &&
			this->Position == other.Position &&
			this->Radius == other.Radius &&
			this->MaterialIndex == other.MaterialIndex
		);
	}
};

struct Scene {
	std::string Name = "New Scene";

	Sky Sky;

	std::vector<Sphere> Spheres;
	std::vector<Material> Materials;
	std::vector<Light> Lights;
};
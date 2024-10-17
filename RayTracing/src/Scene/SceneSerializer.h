#pragma once

#include "Scene.h"

#include <yaml-cpp/yaml.h>

#include <filesystem>

class SceneSerializer {
public:
	SceneSerializer(Scene& scene);

	bool Serialize(const std::filesystem::path& filepath);
	bool Deserialize(const std::filesystem::path& filepath);
private:
	void SerializeSky(YAML::Emitter& out);
	void SerializeLights(YAML::Emitter& out);
	void SerializeSpheres(YAML::Emitter& out);
	void SerializeMaterials(YAML::Emitter& out);

	void SerializeLight(YAML::Emitter& out, const Light& light);
	void SerializeSphere(YAML::Emitter& out, const Sphere& sphere);
	void SerializeMaterial(YAML::Emitter& out, const Material& material);

	void DeserializeSky(YAML::Node& sceneNode);
	void DeserializeLights(YAML::Node& sceneNode);
	void DeserializeSpheres(YAML::Node& sceneNode);
	void DeserializeMaterials(YAML::Node& sceneNode);

	void DeserializeLight(YAML::Node& lightNode);
	void DeserializeSphere(YAML::Node& sphereNode);
	void DeserializeMaterial(YAML::Node& materialNode);
private:
	Scene& m_Scene;
};
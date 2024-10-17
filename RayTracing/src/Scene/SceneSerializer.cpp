#include "SceneSerializer.h"

#include <fstream>

namespace YAML {
	template<>
	struct convert<glm::vec3> {
		static Node encode(const glm::vec3& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs) {
			if (!node.IsSequence() || node.size() != 3) return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v) {
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

SceneSerializer::SceneSerializer(Scene& scene)
	: m_Scene(scene)
{}

bool SceneSerializer::Serialize(const std::filesystem::path & filepath) {
	YAML::Emitter out;
	{
		out << YAML::BeginMap; // Root
		out << YAML::Key << "Scene" << YAML::Value;
		{
			out << YAML::BeginMap; // Scene
			out << YAML::Key << "Name" << YAML::Value << m_Scene.Name;
			{
				SerializeSky(out);
				SerializeLights(out);
				SerializeSpheres(out);
				SerializeMaterials(out);			
			}
			out << YAML::EndMap; // Scene
		}
		out << YAML::EndMap; // Root
	}

	std::ofstream fout(filepath);
	fout << out.c_str();
	
	return true;
}

void SceneSerializer::SerializeSky(YAML::Emitter& out) {
	out << YAML::Key << "Sky" << YAML::Value << YAML::BeginMap;
	out << YAML::Key << "Enabled" << YAML::Value << m_Scene.Sky.Enabled;
	out << YAML::Key << "Color" << YAML::Value << m_Scene.Sky.Color;
	out << YAML::EndMap;
}

void SceneSerializer::SerializeLights(YAML::Emitter& out) {
	out << YAML::Key << "Lights" << YAML::Value << YAML::BeginSeq;
	for (const Light& light : m_Scene.Lights) {
		SerializeLight(out, light);
	}
	out << YAML::EndSeq;
}

void SceneSerializer::SerializeSpheres(YAML::Emitter& out) {
	out << YAML::Key << "Spheres" << YAML::Value << YAML::BeginSeq;
	for (const Sphere& sphere : m_Scene.Spheres) {
		SerializeSphere(out, sphere);
	}
	out << YAML::EndSeq;
}

void SceneSerializer::SerializeMaterials(YAML::Emitter& out) {
	out << YAML::Key << "Materials" << YAML::Value << YAML::BeginSeq;
	for (const Material& material : m_Scene.Materials) {
		SerializeMaterial(out, material);
	}
	out << YAML::EndSeq;
}

void SceneSerializer::SerializeLight(YAML::Emitter& out, const Light& light) {
	out << YAML::BeginMap;
	out << YAML::Key << "Light" << YAML::Value;
	out << YAML::BeginMap;
	out << YAML::Key << "Enabled" << YAML::Value << light.Enabled;
	out << YAML::Key << "Direction" << YAML::Value << light.Direction;
	out << YAML::EndMap;
	out << YAML::EndMap;
}

void SceneSerializer::SerializeSphere(YAML::Emitter& out, const Sphere& sphere) {
	out << YAML::BeginMap;
	out << YAML::Key << "Sphere" << YAML::Value;
	out << YAML::BeginMap;
	out << YAML::Key << "Enabled" << YAML::Value << sphere.Enabled;
	out << YAML::Key << "Position" << YAML::Value << sphere.Position;
	out << YAML::Key << "Radius" << YAML::Value << sphere.Radius;
	out << YAML::Key << "MaterialIndex" << YAML::Value << sphere.MaterialIndex;
	out << YAML::EndMap;
	out << YAML::EndMap;
}

void SceneSerializer::SerializeMaterial(YAML::Emitter& out, const Material& material) {
	out << YAML::BeginMap;
	out << YAML::Key << "Material" << YAML::Value;
	out << YAML::BeginMap;
	out << YAML::Key << "Name" << YAML::Value << material.Name;
	out << YAML::Key << "Albedo" << YAML::Value << material.Albedo;
	out << YAML::Key << "Roughness" << YAML::Value << material.Roughness;
	out << YAML::Key << "Metallic" << YAML::Value << material.Metallic;
	out << YAML::Key << "EmissionColor" << YAML::Value << material.EmissionColor;
	out << YAML::Key << "EmissionPower" << YAML::Value << material.EmissionPower;
	out << YAML::EndMap;
	out << YAML::EndMap;
}

bool SceneSerializer::Deserialize(const std::filesystem::path& filepath) {
	YAML::Node data;
	try {
		data = YAML::LoadFile(filepath.string());
	} catch (YAML::ParserException e) {
		return false;
	}

	if (!data["Scene"]) {
		return false;
	}

	auto sceneNode = data["Scene"];

	m_Scene.Name = sceneNode["Name"].as<std::string>();
	m_Scene.Lights.clear();
	m_Scene.Spheres.clear();
	m_Scene.Materials.clear();

	DeserializeSky(sceneNode);
	DeserializeLights(sceneNode);
	DeserializeSpheres(sceneNode);
	DeserializeMaterials(sceneNode);

	return true;
}

void SceneSerializer::DeserializeSky(YAML::Node& sceneNode) {
	auto& skyNode = sceneNode["Sky"];
	if (skyNode) {
		m_Scene.Sky.Enabled = skyNode["Enabled"].as<bool>();
		m_Scene.Sky.Color = skyNode["Color"].as<glm::vec3>();
	}
}

void SceneSerializer::DeserializeLights(YAML::Node& sceneNode) {
	auto& lightsNode = sceneNode["Lights"];
	if (lightsNode) {
		for (auto& lightNode : lightsNode) {
			DeserializeLight(lightNode);
		}
	}
}

void SceneSerializer::DeserializeSpheres(YAML::Node& sceneNode) {
	auto& spheresNode = sceneNode["Spheres"];
	if (spheresNode) {
		for (auto& sphereNode : spheresNode) {
			DeserializeSphere(sphereNode);
		}
	}
}

void SceneSerializer::DeserializeMaterials(YAML::Node& sceneNode) {
	auto& materialsNode = sceneNode["Materials"];
	if (materialsNode) {
		for (auto& materialNode : materialsNode) {
			DeserializeMaterial(materialNode);
		}
	}
}

void SceneSerializer::DeserializeLight(YAML::Node& lightNode) {
	auto& light = lightNode["Light"];
	Light& newLight = m_Scene.Lights.emplace_back();
	newLight.Enabled = light["Enabled"].as<bool>();
	newLight.Direction = light["Direction"].as<glm::vec3>();
}

void SceneSerializer::DeserializeSphere(YAML::Node& sphereNode) {
	auto& sphere = sphereNode["Sphere"];
	Sphere& newSphere = m_Scene.Spheres.emplace_back();
	newSphere.Enabled = sphere["Enabled"].as<bool>();
	newSphere.Position = sphere["Position"].as<glm::vec3>();
	newSphere.Radius = sphere["Radius"].as<float>();
	newSphere.MaterialIndex = sphere["MaterialIndex"].as<int>();
}

void SceneSerializer::DeserializeMaterial(YAML::Node& materialNode) {
	auto& material = materialNode["Material"];
	Material& newMaterial = m_Scene.Materials.emplace_back();
	newMaterial.Name = material["Name"].as<std::string>();
	newMaterial.Albedo = material["Albedo"].as<glm::vec3>();
	newMaterial.Roughness = material["Roughness"].as<float>();
	newMaterial.Metallic = material["Metallic"].as<float>();
	newMaterial.EmissionColor = material["EmissionColor"].as<glm::vec3>();
	newMaterial.EmissionPower = material["EmissionPower"].as<float>();
}

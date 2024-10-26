#pragma once

#include "../Renderer.h"

#include <yaml-cpp/yaml.h>

#include <filesystem>

class RendererSettingsSerializer {
public:
	RendererSettingsSerializer(Renderer& renderer);

	bool Serialize(const std::filesystem::path& filepath);
	bool Deserialize(const std::filesystem::path& filepath);
private:
	void SerializeSettings(YAML::Emitter& out, const Renderer::Settings& settings);
	void DeserializeSettings(YAML::Node& settingsNode);
private:
	Renderer& m_Renderer;
};
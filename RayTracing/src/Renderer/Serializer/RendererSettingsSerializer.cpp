#include "RendererSettingsSerializer.h"

#include <fstream>

RendererSettingsSerializer::RendererSettingsSerializer(Renderer& renderer)
	: m_Renderer(renderer)
{}

bool RendererSettingsSerializer::Serialize(const std::filesystem::path& filepath) {
	YAML::Emitter out;
	{
		out << YAML::BeginMap; // Root
		out << YAML::Key << "Renderer" << YAML::Value;
		{
			out << YAML::BeginMap; // Renderer
			SerializeSettings(out, m_Renderer.GetSettings());
			out << YAML::EndMap; // Renderer
		}
		out << YAML::EndMap; // Root
	}

	std::ofstream fout(filepath);
	fout << out.c_str();

	return true;
}

void RendererSettingsSerializer::SerializeSettings(YAML::Emitter& out, const Renderer::Settings& settings) {
	out << YAML::Key << "Accumulate" << YAML::Value << settings.Accumulate;
	out << YAML::Key << "Multithreading" << YAML::Value << settings.Multithreading;
	out << YAML::Key << "FastRandom" << YAML::Value << settings.FastRandom;
	out << YAML::Key << "UseClockTime" << YAML::Value << settings.UseClockTime;
	out << YAML::Key << "UseFrameIndex" << YAML::Value << settings.UseFrameIndex;
	out << YAML::Key << "UseRayBounces" << YAML::Value << settings.UseRayBounces;
	out << YAML::Key << "RayBounces" << YAML::Value << settings.RayBounces;
	out << YAML::Key << "ResolutionScale" << YAML::Value << settings.ResolutionScale;
}

bool RendererSettingsSerializer::Deserialize(const std::filesystem::path& filepath) {
	YAML::Node data;
	try {
		data = YAML::LoadFile(filepath.string());
	} catch (YAML::ParserException e) {
		return false;
	}

	if (!data["Renderer"]) {
		return false;
	}

	auto settingsNode = data["Renderer"];

	DeserializeSettings(settingsNode);

	return true;
}

void RendererSettingsSerializer::DeserializeSettings(YAML::Node& settingsNode) {
	auto& settings = m_Renderer.GetSettings();

	settings.Accumulate = settingsNode["Accumulate"].as<bool>();
	settings.Multithreading = settingsNode["Multithreading"].as<bool>();
	settings.FastRandom = settingsNode["FastRandom"].as<bool>();
	settings.UseClockTime = settingsNode["UseClockTime"].as<bool>();
	settings.UseFrameIndex = settingsNode["UseFrameIndex"].as<bool>();
	settings.UseRayBounces = settingsNode["UseRayBounces"].as<bool>();
	settings.RayBounces = settingsNode["RayBounces"].as<int>();
	settings.ResolutionScale = settingsNode["ResolutionScale"].as<int>();
}

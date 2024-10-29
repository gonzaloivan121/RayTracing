#pragma once

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <unordered_map>
#include <string>

class TranslationSerializer {
public:
	TranslationSerializer(std::unordered_map<std::string, std::string>& data);

	bool Deserialize(const std::filesystem::path& filepath);
private:
	void DeserializeLanguage(YAML::Node& languageNode);
private:
	std::unordered_map<std::string, std::string>& m_Data;
};
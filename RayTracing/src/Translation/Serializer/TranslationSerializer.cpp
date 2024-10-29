#include "TranslationSerializer.h"

#include <iostream>

TranslationSerializer::TranslationSerializer(std::unordered_map<std::string, std::string>& data)
	: m_Data(data)
{}

bool TranslationSerializer::Deserialize(const std::filesystem::path& filepath) {
    YAML::Node data;
	try {
		data = YAML::LoadFile(filepath.string());
	} catch (YAML::Exception e) {
		return false;
	}

	if (!data["Language"]) {
		return false;
	}

	auto languageNode = data["Language"];

	DeserializeLanguage(languageNode);

    return true;
}

void TranslationSerializer::DeserializeLanguage(YAML::Node& languageNode) {
	auto translationsNode = languageNode["Translations"];

	for (YAML::const_iterator it = translationsNode.begin(); it != translationsNode.end(); ++it) {
		std::string key = it->first.as<std::string>();
		std::string value = it->second.as<std::string>();

		m_Data[key] = value;
	}
}

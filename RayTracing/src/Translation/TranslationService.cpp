#include "TranslationService.h"

#include "Serializer/TranslationSerializer.h"

#include "Walnut/Core/Log.h"

#include <filesystem>

void TranslationService::Use(std::string language) {
	spdlog::info("TranslationService - Loading " + language + " translations");
	s_CurrentLanguage = language;

	std::filesystem::path languagePath(s_LanguagesPath + s_CurrentLanguage + ".yaml");

	TranslationSerializer serializer(s_Data);
	if (serializer.Deserialize(languagePath)) {
		spdlog::info("TranslationService - " + language + " translations loading complete");
	} else {
		spdlog::error("TranslationService - There's been an error loading " + language + " translations");
	}
}

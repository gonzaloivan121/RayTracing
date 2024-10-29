#pragma once

#include <string>
#include <unordered_map>

class TranslationService {
public:
	static void Use(std::string language);
	static const std::string& Get(std::string key) { return s_Data[key]; }
public:
	static const std::string GetCurrentLanguage() { return s_CurrentLanguage; }
	static const std::unordered_map<std::string, std::string> GetData() { return s_Data; }
private:
	inline static const std::string s_LanguagesPath = "i18n/";

	inline static std::unordered_map<std::string, std::string> s_Data;
	inline static std::string s_CurrentLanguage = "English";
};
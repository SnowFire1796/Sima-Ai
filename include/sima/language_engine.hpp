#pragma once
#include "sima/text.hpp"
namespace sima {
struct ParsedText { Language language{Language::Unknown}; std::string normalized; std::vector<std::string> tokens; };
class LanguageEngine {
public: ParsedText analyze(std::string_view input)const{return{ text::detect_language(input),text::normalize(input),text::tokenize(input)}; }
};
}

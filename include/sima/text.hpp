#pragma once
#include "sima/types.hpp"
#include <string>
#include <string_view>
#include <vector>
namespace sima::text {
std::string normalize(std::string_view input);
std::vector<std::string> tokenize(std::string_view input);
Language detect_language(std::string_view input);
std::string trim(std::string_view input);
bool is_valid_utf8(std::string_view input);
}

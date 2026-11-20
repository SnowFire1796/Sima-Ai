#pragma once
#include "sima/storage.hpp"
#include "sima/types.hpp"
#include <filesystem>
#include <string>
namespace sima {
class TextFactExtractor {
public:
 static TrainReport train_file(const std::filesystem::path&,KnowledgeBase&,std::size_t max_line_bytes=1024U*1024U);
 static TrainReport train_text(const std::string&,KnowledgeBase&,std::size_t max_line_bytes=1024U*1024U);
};
}

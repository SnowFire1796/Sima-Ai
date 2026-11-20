#pragma once
#include "sima/dataset.hpp"
#include "sima/types.hpp"
#include <filesystem>
namespace sima {
class LearningEngine {
public:
 explicit LearningEngine(std::size_t max_line_bytes=1024U*1024U):max_line_bytes_(max_line_bytes){}
 TrainReport train_path(const std::filesystem::path&path,KnowledgeBase&knowledge,bool recursive=true)const;
private: std::size_t max_line_bytes_;
};
}

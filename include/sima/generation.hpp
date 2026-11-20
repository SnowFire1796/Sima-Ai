#pragma once
#include "sima/types.hpp"
#include <string>
#include <vector>
namespace sima {
class GenerationEngine {
public: Answer compose(const std::vector<SearchHit>&hits)const;
 std::string explain(const std::vector<Fact>&evidence)const;
};
}

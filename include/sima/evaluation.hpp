#pragma once
#include "sima/types.hpp"
#include <string>
namespace sima {
struct Evaluation { bool has_evidence{false}; bool sufficient{false}; double coverage{0.0}; std::string note; };
class EvaluationEngine {
public: Evaluation evaluate(const std::vector<std::string>&query_terms,const std::vector<SearchHit>&hits)const;
};
}

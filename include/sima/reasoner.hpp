#pragma once
#include "sima/storage.hpp"
#include <vector>
namespace sima {
class Reasoner {
public:
 explicit Reasoner(const KnowledgeBase&k):knowledge_(k){}
 std::vector<Fact> infer(std::size_t max_new_facts=10000)const;
 bool explain(ConceptId subject,const std::string&relation,ConceptId object,std::vector<Fact>&evidence)const;
private: const KnowledgeBase&knowledge_;
};
}

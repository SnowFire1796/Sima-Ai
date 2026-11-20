#pragma once
#include "sima/concepts.hpp"
#include "sima/types.hpp"
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>
namespace sima {
class KnowledgeBase {
public:
 ConceptStore concepts; std::vector<Fact> facts; std::vector<Rule> rules;
 bool add_fact(Fact fact); bool add_rule(Rule rule); void rebuild_indexes();
 std::vector<const Fact*> find(ConceptId subject,const std::string& relation)const;
 bool contains(ConceptId subject,const std::string& relation,ConceptId object)const;
 KnowledgeStats stats()const; void clear();
 bool save(const std::filesystem::path& path,std::string& error)const;
 bool load(const std::filesystem::path& path,std::string& error);
private:
 std::unordered_multimap<ConceptId,std::size_t> by_subject_;
};
}

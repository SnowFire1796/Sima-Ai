#pragma once
#include "sima/concepts.hpp"
#include "sima/types.hpp"
#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
namespace sima {
class KnowledgeBase {
public:
 ConceptStore concepts;
 std::vector<Fact> facts;
 std::vector<Rule> rules;
 std::vector<ResponsePair> responses;
 bool add_fact(Fact fact);
 bool add_rule(Rule rule);
 bool add_response(ResponsePair response);
 void rebuild_indexes();
 std::vector<const Fact*> find(ConceptId subject,const std::string& relation)const;
 const ResponsePair* find_response(std::string_view question)const;
 bool contains(ConceptId subject,const std::string& relation,ConceptId object)const;
 KnowledgeStats stats()const;
 void clear();
 bool save(const std::filesystem::path& path,std::string& error)const;
 bool load(const std::filesystem::path& path,std::string& error);
private:
 std::unordered_multimap<ConceptId,std::size_t> by_subject_;
 std::unordered_multimap<std::string,std::size_t> responses_by_question_;
};
}

#include "sima/retrieval.hpp"
#include "sima/text.hpp"
#include <algorithm>
#include <unordered_set>
#include <utility>
namespace sima {
void RetrievalIndex::rebuild(const KnowledgeBase&knowledge){
 postings_.clear();
 for(std::size_t i=0;i<knowledge.facts.size();++i){
  const auto&fact=knowledge.facts[i];std::unordered_set<std::string>terms;
  for(const auto&field:{fact.subject_label,fact.relation,fact.object_label,fact.source})for(const auto&term:text::tokenize(field))terms.insert(term);
  const auto subject=knowledge.concepts.all().find(fact.subject);if(subject!=knowledge.concepts.all().end())for(const auto&alias:subject->second.aliases)for(const auto&term:text::tokenize(alias))terms.insert(term);
  const auto object=knowledge.concepts.all().find(fact.object);if(object!=knowledge.concepts.all().end())for(const auto&alias:object->second.aliases)for(const auto&term:text::tokenize(alias))terms.insert(term);
  for(const auto&term:terms)postings_[term].push_back(i);
 }
}
std::vector<SearchHit>RetrievalIndex::search(std::string_view query,const KnowledgeBase&knowledge,std::size_t limit)const{
 const auto terms=text::tokenize(query);std::unordered_map<std::size_t,double>scores;std::unordered_map<std::size_t,std::unordered_set<std::string>>matched;
 for(const auto&term:terms){const auto found=postings_.find(term);if(found==postings_.end())continue;const double weight=1.0/static_cast<double>(found->second.size()+1);for(const auto index:found->second){scores[index]+=weight;matched[index].insert(term);}}
 std::vector<SearchHit>hits;for(const auto&entry:scores)if(entry.first<knowledge.facts.size()){SearchHit hit;hit.fact=knowledge.facts[entry.first];hit.score=entry.second;const auto found=matched.find(entry.first);if(found!=matched.end()){hit.matched_terms.assign(found->second.begin(),found->second.end());std::sort(hit.matched_terms.begin(),hit.matched_terms.end());}hits.push_back(std::move(hit));}
 std::sort(hits.begin(),hits.end(),[](const SearchHit&a,const SearchHit&b){if(a.score!=b.score)return a.score>b.score;if(a.fact.subject_label!=b.fact.subject_label)return a.fact.subject_label<b.fact.subject_label;if(a.fact.relation!=b.fact.relation)return a.fact.relation<b.fact.relation;if(a.fact.object_label!=b.fact.object_label)return a.fact.object_label<b.fact.object_label;return a.fact.source<b.fact.source;});
 if(hits.size()>limit)hits.resize(limit);return hits;
}
}

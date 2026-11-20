#include "sima/retrieval.hpp"
#include "sima/text.hpp"
#include <algorithm>
#include <unordered_set>
namespace sima {
void RetrievalIndex::rebuild(const KnowledgeBase&k){postings_.clear();for(std::size_t i=0;i<k.facts.size();++i){const auto&f=k.facts[i];std::unordered_set<std::string>terms;for(const auto&s:{f.subject_label,f.relation,f.object_label,f.source})for(const auto&t:text::tokenize(s))terms.insert(t);for(const auto&t:terms)postings_[t].push_back(i);}}
std::vector<SearchHit> RetrievalIndex::search(std::string_view q,const KnowledgeBase&k,std::size_t limit)const{auto terms=text::tokenize(q);std::unordered_map<std::size_t,double>scores;for(const auto&t:terms){auto it=postings_.find(t);if(it==postings_.end())continue;double w=1.0/static_cast<double>(it->second.size()+1);for(auto id:it->second)scores[id]+=w;}std::vector<SearchHit>hits;for(const auto&[id,score]:scores)if(id<k.facts.size())hits.push_back({k.facts[id],score,false});std::sort(hits.begin(),hits.end(),[](const auto&a,const auto&b){if(a.score!=b.score)return a.score>b.score;if(a.fact.subject_label!=b.fact.subject_label)return a.fact.subject_label<b.fact.subject_label;return a.fact.object_label<b.fact.object_label;});if(hits.size()>limit)hits.resize(limit);return hits;}
}

#pragma once
#include "sima/text.hpp"
#include <limits>
#include <utility>
#include <unordered_map>
namespace sima {
class ConceptStore {
public:
 ConceptId ensure(std::string_view canonical){const auto key=text::normalize(canonical);if(key.empty()||next_==0)return 0;auto it=aliases_.find(key);if(it!=aliases_.end())return it->second;const auto id=next_;next_=(next_==std::numeric_limits<ConceptId>::max())?0:next_+1;Concept c{id,std::string(canonical),{std::string(canonical)}};concepts_.emplace(id,std::move(c));aliases_.emplace(key,id);return id;}
 bool add_alias(std::string_view alias,std::string_view canonical){const auto ak=text::normalize(alias),ck=text::normalize(canonical);if(ak.empty()||ck.empty())return false;if(ak==ck)return ensure(canonical)!=0;const auto ai=aliases_.find(ak),ci=aliases_.find(ck);if(ai!=aliases_.end())return ci!=aliases_.end()&&ai->second==ci->second;const auto id=ensure(canonical);if(!id)return false;aliases_.emplace(ak,id);concepts_.at(id).aliases.emplace_back(alias);return true;}
 ConceptId resolve(std::string_view value)const{auto it=aliases_.find(text::normalize(value));return it==aliases_.end()?0:it->second;}
 std::string label(ConceptId id)const{auto it=concepts_.find(id);return it==concepts_.end()?std::string{}:it->second.canonical;}
 const auto&all()const{return concepts_;}std::size_t size()const{return concepts_.size();}
 void clear(){concepts_.clear();aliases_.clear();next_=1;}
 bool restore(ConceptId id,const std::string&canonical,const std::vector<std::string>&names){if(!id||canonical.empty()||concepts_.contains(id))return false;std::vector<std::string>all_names=names;if(all_names.empty())all_names.push_back(canonical);all_names.push_back(canonical);for(const auto&name:all_names){const auto key=text::normalize(name);if(key.empty())return false;auto it=aliases_.find(key);if(it!=aliases_.end()&&it->second!=id)return false;}Concept c{id,canonical,names};if(c.aliases.empty())c.aliases.push_back(canonical);concepts_.emplace(id,c);for(const auto&name:all_names)aliases_.try_emplace(text::normalize(name),id);if(id>=next_)next_=(id==std::numeric_limits<ConceptId>::max())?0:id+1;return true;}
private: ConceptId next_{1};std::unordered_map<ConceptId,Concept>concepts_;std::unordered_map<std::string,ConceptId>aliases_;
};
}

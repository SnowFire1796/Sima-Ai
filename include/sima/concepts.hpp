#pragma once
#include "sima/text.hpp"
#include <unordered_map>
namespace sima {
class ConceptStore {
public:
 ConceptId ensure(std::string_view canonical){auto key=text::normalize(canonical);if(key.empty())return 0;auto it=aliases_.find(key);if(it!=aliases_.end())return it->second;auto id=next_++;Concept c{id,std::string(canonical),{std::string(canonical)}};concepts_.emplace(id,std::move(c));aliases_.emplace(key,id);return id;}
 bool add_alias(std::string_view alias,std::string_view canonical){auto key=text::normalize(alias);if(key.empty())return false;auto id=ensure(canonical);auto it=aliases_.find(key);if(it!=aliases_.end())return it->second==id;aliases_[key]=id;concepts_.at(id).aliases.emplace_back(alias);return true;}
 ConceptId resolve(std::string_view value)const{auto it=aliases_.find(text::normalize(value));return it==aliases_.end()?0:it->second;}
 std::string label(ConceptId id)const{auto it=concepts_.find(id);return it==concepts_.end()?std::string{}:it->second.canonical;}
 const auto& all()const{return concepts_;} std::size_t size()const{return concepts_.size();}
 void clear(){concepts_.clear();aliases_.clear();next_=1;}
 bool restore(ConceptId id,const std::string& canonical,const std::vector<std::string>& names){if(!id||canonical.empty()||concepts_.contains(id))return false;Concept c{id,canonical,names};if(c.aliases.empty())c.aliases.push_back(canonical);concepts_.emplace(id,c);for(const auto& a:c.aliases){auto k=text::normalize(a);if(!k.empty())aliases_.try_emplace(k,id);}aliases_.try_emplace(text::normalize(canonical),id);if(id>=next_)next_=id+1;return true;}
private: ConceptId next_{1};std::unordered_map<ConceptId,Concept> concepts_;std::unordered_map<std::string,ConceptId> aliases_;
};
}

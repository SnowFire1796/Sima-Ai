#pragma once
#include "sima/text.hpp"
#include <limits>
#include <unordered_map>
#include <utility>
namespace sima {
class ConceptStore {
public:
 ConceptId ensure(std::string_view canonical){if(!text::is_valid_utf8(canonical))return 0;const auto key=text::normalize(canonical);if(key.empty()||next_==0)return 0;const auto it=aliases_.find(key);if(it!=aliases_.end())return it->second;const auto id=next_;next_=(next_==std::numeric_limits<ConceptId>::max())?0:next_+1;Concept entry{id,std::string(canonical),{std::string(canonical)}};concepts_.emplace(id,std::move(entry));aliases_.emplace(key,id);return id;}
 bool add_alias(std::string_view alias,std::string_view canonical){if(!text::is_valid_utf8(alias)||!text::is_valid_utf8(canonical))return false;const auto alias_key=text::normalize(alias),canonical_key=text::normalize(canonical);if(alias_key.empty()||canonical_key.empty())return false;if(alias_key==canonical_key)return ensure(canonical)!=0;const auto alias_it=aliases_.find(alias_key),canonical_it=aliases_.find(canonical_key);if(alias_it!=aliases_.end())return canonical_it!=aliases_.end()&&alias_it->second==canonical_it->second;const auto id=ensure(canonical);if(!id)return false;aliases_.emplace(alias_key,id);concepts_.at(id).aliases.emplace_back(alias);return true;}
 ConceptId resolve(std::string_view value)const{if(!text::is_valid_utf8(value))return 0;const auto it=aliases_.find(text::normalize(value));return it==aliases_.end()?0:it->second;}
 std::string label(ConceptId id)const{const auto it=concepts_.find(id);return it==concepts_.end()?std::string{}:it->second.canonical;}
 const auto&all()const{return concepts_;}std::size_t size()const{return concepts_.size();}
 void clear(){concepts_.clear();aliases_.clear();next_=1;}
 bool restore(ConceptId id,const std::string&canonical,const std::vector<std::string>&names){if(!id||canonical.empty()||!text::is_valid_utf8(canonical)||concepts_.contains(id))return false;std::vector<std::string>all_names=names;if(all_names.empty())all_names.push_back(canonical);all_names.push_back(canonical);for(const auto&name:all_names){if(!text::is_valid_utf8(name))return false;const auto key=text::normalize(name);if(key.empty())return false;const auto it=aliases_.find(key);if(it!=aliases_.end()&&it->second!=id)return false;}Concept entry{id,canonical,names};if(entry.aliases.empty())entry.aliases.push_back(canonical);concepts_.emplace(id,entry);for(const auto&name:all_names)aliases_.try_emplace(text::normalize(name),id);if(next_!=0&&id>=next_)next_=(id==std::numeric_limits<ConceptId>::max())?0:id+1;return true;}
private:ConceptId next_{1};std::unordered_map<ConceptId,Concept>concepts_;std::unordered_map<std::string,ConceptId>aliases_;
};
}

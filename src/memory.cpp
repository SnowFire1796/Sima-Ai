#include "sima/memory.hpp"
#include "sima/text.hpp"
#include <algorithm>
#include <unordered_set>
#include <utility>
namespace sima {
void MemoryEngine::remember(std::string input,std::string response){if(capacity_==0)return;entries_.push_back({std::move(input),std::move(response),next_++});while(entries_.size()>capacity_)entries_.pop_front();}
std::vector<MemoryEntry>MemoryEngine::recent(std::size_t limit)const{std::vector<MemoryEntry>out;for(auto it=entries_.rbegin();it!=entries_.rend()&&out.size()<limit;++it)out.push_back(*it);return out;}
std::vector<MemoryEntry>MemoryEngine::search(std::string_view query,std::size_t limit)const{const auto terms=text::tokenize(query);std::vector<std::pair<std::size_t,const MemoryEntry*>>ranked;for(const auto&entry:entries_){const auto tokens=text::tokenize(entry.input+" "+entry.response);const std::unordered_set<std::string>available(tokens.begin(),tokens.end());std::size_t score=0;for(const auto&term:terms)if(available.contains(term))++score;if(score)ranked.emplace_back(score,&entry);}std::sort(ranked.begin(),ranked.end(),[](const auto&a,const auto&b){return a.first!=b.first?a.first>b.first:a.second->sequence>b.second->sequence;});std::vector<MemoryEntry>out;for(std::size_t i=0;i<ranked.size()&&i<limit;++i)out.push_back(*ranked[i].second);return out;}
void MemoryEngine::clear(){entries_.clear();next_=1;}
}

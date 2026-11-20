#include "sima/memory.hpp"
#include "sima/text.hpp"
#include <algorithm>
#include <unordered_set>
namespace sima {
void MemoryEngine::remember(std::string in,std::string out){if(capacity_==0)return;entries_.push_back({std::move(in),std::move(out),next_++});while(entries_.size()>capacity_)entries_.pop_front();}
std::vector<MemoryEntry>MemoryEngine::recent(std::size_t n)const{std::vector<MemoryEntry>v;for(auto i=entries_.rbegin();i!=entries_.rend()&&v.size()<n;++i)v.push_back(*i);return v;}
std::vector<MemoryEntry>MemoryEngine::search(std::string_view q,std::size_t n)const{auto terms=text::tokenize(q);std::vector<std::pair<std::size_t,const MemoryEntry*>>ranked;for(const auto&e:entries_){auto hay=text::tokenize(e.input+" "+e.response);std::unordered_set<std::string>set(hay.begin(),hay.end());std::size_t score=0;for(const auto&t:terms)if(set.contains(t))++score;if(score)ranked.emplace_back(score,&e);}std::sort(ranked.begin(),ranked.end(),[](auto&a,auto&b){return a.first!=b.first?a.first>b.first:a.second->sequence>b.second->sequence;});std::vector<MemoryEntry>v;for(std::size_t i=0;i<ranked.size()&&i<n;++i)v.push_back(*ranked[i].second);return v;}
void MemoryEngine::clear(){entries_.clear();next_=1;}
}

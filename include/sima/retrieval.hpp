#pragma once
#include "sima/storage.hpp"
#include "sima/types.hpp"
#include <string_view>
#include <unordered_map>
#include <vector>
namespace sima {
class RetrievalIndex {
public:
 void rebuild(const KnowledgeBase&);
 std::vector<SearchHit> search(std::string_view,const KnowledgeBase&,std::size_t limit=10)const;
 std::size_t term_count()const{return postings_.size();}
private: std::unordered_map<std::string,std::vector<std::size_t>>postings_;
};
}

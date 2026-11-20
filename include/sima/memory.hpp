#pragma once
#include "sima/types.hpp"
#include <cstddef>
#include <deque>
#include <string>
#include <string_view>
#include <vector>
namespace sima {
struct MemoryEntry { std::string input; std::string response; std::uint64_t sequence{0}; };
class MemoryEngine {
public:
 explicit MemoryEngine(std::size_t capacity=128):capacity_(capacity){}
 void remember(std::string input,std::string response);
 std::vector<MemoryEntry> recent(std::size_t limit=10)const;
 std::vector<MemoryEntry> search(std::string_view query,std::size_t limit=10)const;
 void clear(); std::size_t size()const{return entries_.size();}
private: std::size_t capacity_;std::uint64_t next_{1};std::deque<MemoryEntry>entries_;
};
}

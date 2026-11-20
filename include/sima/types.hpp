#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
namespace sima {
using ConceptId=std::uint64_t;
enum class Language { Unknown, English, Persian };
enum class FactOrigin : std::uint8_t { Dataset=0, User=1, Inference=2 };
struct Concept { ConceptId id{0}; std::string canonical; std::vector<std::string> aliases; };
struct Fact {
 ConceptId subject{0}; std::string subject_label; std::string relation;
 ConceptId object{0}; std::string object_label; std::string source;
 double confidence{1.0}; FactOrigin origin{FactOrigin::Dataset}; std::uint64_t created_at{0};
};
struct ResponsePair { std::string question; std::string answer; std::string source; double confidence{1.0}; };
struct Rule { std::string relation; bool transitive{false}; };
struct Diagnostic { std::size_t line{0}; std::string message; };
struct TrainReport {
 std::size_t lines_read{0},facts_added{0},aliases_added{0},rules_added{0},responses_added{0},duplicates{0};
 std::size_t sentences_seen{0},sentences_extracted{0},sentences_unmatched{0};
 std::vector<Diagnostic> diagnostics;
};
struct SearchHit { Fact fact; double score{0}; bool inferred{false}; std::vector<std::string> matched_terms; };
struct Answer { std::string text; std::vector<Fact> evidence; bool sufficient{false}; std::string source; };
struct KnowledgeStats { std::size_t concepts{0},facts{0},rules{0},responses{0},indexed_terms{0}; };
}

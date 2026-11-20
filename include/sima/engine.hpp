#pragma once
#include "sima/dataset.hpp"
#include "sima/evaluation.hpp"
#include "sima/generation.hpp"
#include "sima/memory.hpp"
#include "sima/reasoner.hpp"
#include "sima/retrieval.hpp"
#include <utility>
namespace sima {
class SimaEngine {
public:
 TrainReport train(const std::filesystem::path&);
 TrainReport train_text(const std::string&);
 std::vector<const Fact*> ask(std::string_view subject,std::string_view relation)const;
 std::vector<Fact> infer(std::size_t limit=10000)const;
 Answer query(std::string_view,std::size_t limit=5);
 bool explain(std::string_view,std::string_view,std::string_view,std::vector<Fact>&)const;
 bool save(const std::filesystem::path&,std::string&)const;
 bool load(const std::filesystem::path&,std::string&);
 KnowledgeStats stats()const;
 const KnowledgeBase&knowledge()const{return knowledge_;}
 const MemoryEngine&memory()const{return memory_;}
 void remember(std::string input,std::string response){memory_.remember(std::move(input),std::move(response));}
 void rebuild();
private:
 KnowledgeBase knowledge_;RetrievalIndex index_;GenerationEngine generator_;EvaluationEngine evaluator_;MemoryEngine memory_;
};
std::string format_fact(const Fact&);
}

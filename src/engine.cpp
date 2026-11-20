#include "sima/engine.hpp"
#include "sima/extractor.hpp"
#include "sima/text.hpp"
namespace sima {
TrainReport SimaEngine::train(const std::filesystem::path&path){auto report=learner_.train_path(path,knowledge_);rebuild();return report;}
TrainReport SimaEngine::train_text(const std::string&content){auto report=TextFactExtractor::train_text(content,knowledge_);rebuild();return report;}
std::vector<const Fact*>SimaEngine::ask(std::string_view subject,std::string_view relation)const{const auto id=knowledge_.concepts.resolve(subject);return id?knowledge_.find(id,text::trim(relation)):std::vector<const Fact*>{};}
std::vector<Fact>SimaEngine::infer(std::size_t limit)const{return Reasoner(knowledge_).infer(limit);}
Answer SimaEngine::query(std::string_view query_text,std::size_t limit){const auto hits=index_.search(query_text,knowledge_,limit);auto answer=generator_.compose(hits);const auto evaluation=evaluator_.evaluate(text::tokenize(query_text),hits);if(!evaluation.sufficient){answer.sufficient=false;if(!hits.empty())answer.text="I found possible matches, but the evidence overlap is weak. Treat these as leads, not a confirmed answer.\n"+answer.text;}memory_.remember(std::string(query_text),answer.text);return answer;}
bool SimaEngine::explain(std::string_view subject,std::string_view relation,std::string_view object,std::vector<Fact>&evidence)const{const auto s=knowledge_.concepts.resolve(subject),o=knowledge_.concepts.resolve(object);if(!s||!o){evidence.clear();return false;}return Reasoner(knowledge_).explain(s,text::trim(relation),o,evidence);}
bool SimaEngine::save(const std::filesystem::path&path,std::string&error)const{return knowledge_.save(path,error);}
bool SimaEngine::load(const std::filesystem::path&path,std::string&error){if(!knowledge_.load(path,error))return false;rebuild();return true;}
KnowledgeStats SimaEngine::stats()const{auto result=knowledge_.stats();result.indexed_terms=index_.term_count();return result;}
void SimaEngine::rebuild(){knowledge_.rebuild_indexes();index_.rebuild(knowledge_);}
std::string format_fact(const Fact&fact){return fact.subject_label+" --"+fact.relation+"--> "+fact.object_label+(fact.origin==FactOrigin::Inference?" [inferred]":"")+(fact.source.empty()?"":" {"+fact.source+"}");}
}

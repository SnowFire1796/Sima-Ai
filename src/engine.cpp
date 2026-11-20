#include "sima/engine.hpp"
#include "sima/text.hpp"
namespace sima {
TrainReport SimaEngine::train(const std::filesystem::path&p){auto r=DatasetReader::train_file(p,knowledge_);rebuild();return r;}
TrainReport SimaEngine::train_text(const std::string&s){auto r=DatasetReader::train_text(s,knowledge_);rebuild();return r;}
std::vector<const Fact*>SimaEngine::ask(std::string_view s,std::string_view r)const{auto id=knowledge_.concepts.resolve(s);return id?knowledge_.find(id,text::trim(r)):std::vector<const Fact*>{};}
Answer SimaEngine::query(std::string_view q,std::size_t n){auto hits=index_.search(q,knowledge_,n);auto a=generator_.compose(hits);auto evaluation=evaluator_.evaluate(text::tokenize(q),hits);if(!evaluation.sufficient){a.sufficient=false;if(!hits.empty())a.text="I found possible matches, but the evidence overlap is weak. Treat these as leads, not a confirmed answer.\n"+a.text;}memory_.remember(std::string(q),a.text);return a;}
bool SimaEngine::explain(std::string_view s,std::string_view r,std::string_view o,std::vector<Fact>&e)const{auto si=knowledge_.concepts.resolve(s),oi=knowledge_.concepts.resolve(o);if(!si||!oi){e.clear();return false;}return Reasoner(knowledge_).explain(si,text::trim(r),oi,e);}
bool SimaEngine::save(const std::filesystem::path&p,std::string&e)const{return knowledge_.save(p,e);}
bool SimaEngine::load(const std::filesystem::path&p,std::string&e){if(!knowledge_.load(p,e))return false;rebuild();return true;}
KnowledgeStats SimaEngine::stats()const{auto s=knowledge_.stats();s.indexed_terms=index_.term_count();return s;}
void SimaEngine::rebuild(){knowledge_.rebuild_indexes();index_.rebuild(knowledge_);}
std::string format_fact(const Fact&f){return f.subject_label+" --"+f.relation+"--> "+f.object_label+(f.origin==FactOrigin::Inference?" [inferred]":"")+(f.source.empty()?"":" {"+f.source+"}");}
}

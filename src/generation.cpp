#include "sima/generation.hpp"
#include <sstream>
namespace sima {
Answer GenerationEngine::compose(const std::vector<SearchHit>&hits)const{Answer a;if(hits.empty()){a.text="I do not have enough evidence in my knowledge base to answer that.";return a;}std::ostringstream out;out<<"I found "<<hits.size()<<" relevant piece(s) of evidence:";for(const auto&h:hits){a.evidence.push_back(h.fact);out<<"\n- "<<h.fact.subject_label<<" --"<<h.fact.relation<<"--> "<<h.fact.object_label;}a.text=out.str();a.sufficient=true;return a;}
std::string GenerationEngine::explain(const std::vector<Fact>&e)const{if(e.empty())return"No evidence is available.";std::ostringstream out;out<<"Evidence chain:";for(const auto&f:e)out<<"\n"<<f.subject_label<<" --"<<f.relation<<"--> "<<f.object_label<<" ["<<f.source<<"]";return out.str();}
}

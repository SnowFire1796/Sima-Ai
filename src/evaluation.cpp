#include "sima/evaluation.hpp"
#include "sima/text.hpp"
#include <unordered_set>
namespace sima {
Evaluation EvaluationEngine::evaluate(const std::vector<std::string>&terms,const std::vector<SearchHit>&hits)const{Evaluation e;e.has_evidence=!hits.empty();if(terms.empty()){e.note="Query contains no searchable terms.";return e;}std::unordered_set<std::string>covered;for(const auto&h:hits){std::unordered_set<std::string>fact_terms;for(const auto&s:{h.fact.subject_label,h.fact.relation,h.fact.object_label})for(const auto&t:text::tokenize(s))fact_terms.insert(t);for(const auto&t:terms)if(fact_terms.contains(t))covered.insert(t);}e.coverage=static_cast<double>(covered.size())/static_cast<double>(terms.size());e.sufficient=e.has_evidence&&e.coverage>=0.25;e.note=e.sufficient?"Retrieved evidence overlaps the query.":"Evidence coverage is too low for a confident answer.";return e;}
}

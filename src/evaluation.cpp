#include "sima/evaluation.hpp"
#include "sima/text.hpp"
#include <unordered_set>
namespace sima {
Evaluation EvaluationEngine::evaluate(const std::vector<std::string>&terms,const std::vector<SearchHit>&hits)const{
 Evaluation result;result.has_evidence=!hits.empty();if(terms.empty()){result.note="Query contains no searchable terms.";return result;}
 std::unordered_set<std::string>covered;
 for(const auto&hit:hits){for(const auto&term:hit.matched_terms)covered.insert(term);std::unordered_set<std::string>fact_terms;for(const auto&field:{hit.fact.subject_label,hit.fact.relation,hit.fact.object_label})for(const auto&term:text::tokenize(field))fact_terms.insert(term);for(const auto&term:terms)if(fact_terms.contains(term))covered.insert(term);}
 result.coverage=static_cast<double>(covered.size())/static_cast<double>(terms.size());result.sufficient=result.has_evidence&&result.coverage>=0.25;result.note=result.sufficient?"Retrieved evidence overlaps the query.":"Evidence coverage is too low for a confident answer.";return result;
}
}

#include "sima/reasoner.hpp"
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <unordered_set>
namespace sima {
std::vector<Fact> Reasoner::infer(std::size_t cap)const{
 std::vector<Fact>derived;if(cap==0)return derived;
 std::unordered_set<std::string>known;
 for(const auto&f:knowledge_.facts)known.insert(std::to_string(f.subject)+"#"+f.relation+"#"+std::to_string(f.object));
 for(const auto&rule:knowledge_.rules){
  if(!rule.transitive||derived.size()>=cap)continue;
  std::unordered_map<ConceptId,std::vector<const Fact*>>adjacency;
  for(const auto&fact:knowledge_.facts)if(fact.relation==rule.relation)adjacency[fact.subject].push_back(&fact);
  std::vector<ConceptId>starts;starts.reserve(adjacency.size());
  for(auto&entry:adjacency){starts.push_back(entry.first);std::sort(entry.second.begin(),entry.second.end(),[](const Fact*a,const Fact*b){return a->object<b->object;});}
  std::sort(starts.begin(),starts.end());
  for(const auto start:starts){
   std::queue<std::pair<ConceptId,double>>pending;std::unordered_set<ConceptId>seen;
   pending.emplace(start,1.0);seen.insert(start);
   while(!pending.empty()&&derived.size()<cap){
    const auto[current,path_confidence]=pending.front();pending.pop();
    const auto found=adjacency.find(current);if(found==adjacency.end())continue;
    for(const auto*edge:found->second){
     const auto target=edge->object;if(target==start||!seen.insert(target).second)continue;
     const auto key=std::to_string(start)+"#"+rule.relation+"#"+std::to_string(target);
     const double confidence=std::min(path_confidence,edge->confidence);
     if(!known.contains(key)){
      known.insert(key);
      derived.push_back({start,knowledge_.concepts.label(start),rule.relation,target,knowledge_.concepts.label(target),"inference: transitive",confidence,FactOrigin::Inference,0});
     }
     pending.emplace(target,confidence);if(derived.size()>=cap)break;
    }
   }
  }
 }
 return derived;
}
bool Reasoner::explain(ConceptId subject,const std::string&relation,ConceptId object,std::vector<Fact>&evidence)const{
 evidence.clear();
 for(const auto&fact:knowledge_.facts)if(fact.subject==subject&&fact.relation==relation&&fact.object==object){evidence.push_back(fact);return true;}
 bool transitive=false;for(const auto&rule:knowledge_.rules)if(rule.relation==relation&&rule.transitive){transitive=true;break;}if(!transitive)return false;
 std::unordered_map<ConceptId,const Fact*>parent;std::queue<ConceptId>pending;std::unordered_set<ConceptId>seen;
 pending.push(subject);seen.insert(subject);
 while(!pending.empty()){
  const auto current=pending.front();pending.pop();
  for(const auto&fact:knowledge_.facts){
   if(fact.subject!=current||fact.relation!=relation)continue;
   if(seen.insert(fact.object).second){
    parent[fact.object]=&fact;
    if(fact.object==object){auto node=object;while(node!=subject){const auto*edge=parent.at(node);evidence.push_back(*edge);node=edge->subject;}std::reverse(evidence.begin(),evidence.end());return true;}
    pending.push(fact.object);
   }
  }
 }
 return false;
}
}

#include "sima/reasoner.hpp"
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <unordered_set>
namespace sima {
std::vector<Fact> Reasoner::infer(std::size_t cap)const{
 std::vector<Fact>out;std::unordered_set<std::string>known;
 for(const auto&f:knowledge_.facts)known.insert(std::to_string(f.subject)+"#"+f.relation+"#"+std::to_string(f.object));
 for(const auto&rule:knowledge_.rules){
  if(!rule.transitive||out.size()>=cap)continue;
  std::unordered_map<ConceptId,std::vector<const Fact*>>adj;
  for(const auto&f:knowledge_.facts)if(f.relation==rule.relation)adj[f.subject].push_back(&f);
  std::vector<ConceptId>starts;starts.reserve(adj.size());for(const auto&entry:adj)starts.push_back(entry.first);
  for(const auto start:starts){
   std::queue<ConceptId>pending;std::unordered_set<ConceptId>seen;pending.push(start);seen.insert(start);
   while(!pending.empty()&&out.size()<cap){
    const auto current=pending.front();pending.pop();const auto it=adj.find(current);if(it==adj.end())continue;
    for(const auto*edge:it->second){
     const auto target=edge->object;if(target==start||!seen.insert(target).second)continue;
     const auto key=std::to_string(start)+"#"+rule.relation+"#"+std::to_string(target);
     if(!known.contains(key)){known.insert(key);out.push_back({start,knowledge_.concepts.label(start),rule.relation,target,knowledge_.concepts.label(target),"inference: transitive",edge->confidence,FactOrigin::Inference,0});}
     pending.push(target);if(out.size()>=cap)break;
    }
   }
  }
 }
 return out;
}
bool Reasoner::explain(ConceptId subject,const std::string&relation,ConceptId object,std::vector<Fact>&evidence)const{
 evidence.clear();
 for(const auto&f:knowledge_.facts)if(f.subject==subject&&f.relation==relation&&f.object==object){evidence.push_back(f);return true;}
 bool transitive=false;for(const auto&r:knowledge_.rules)if(r.relation==relation&&r.transitive){transitive=true;break;}if(!transitive)return false;
 std::unordered_map<ConceptId,const Fact*>parent;std::queue<ConceptId>pending;std::unordered_set<ConceptId>seen;pending.push(subject);seen.insert(subject);
 while(!pending.empty()){
  const auto current=pending.front();pending.pop();
  for(const auto&f:knowledge_.facts){
   if(f.subject!=current||f.relation!=relation)continue;
   if(seen.insert(f.object).second){parent[f.object]=&f;if(f.object==object){auto node=object;while(node!=subject){const auto*edge=parent.at(node);evidence.push_back(*edge);node=edge->subject;}std::reverse(evidence.begin(),evidence.end());return true;}pending.push(f.object);}
  }
 }
 return false;
}
}

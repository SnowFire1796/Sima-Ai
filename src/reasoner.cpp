#include "sima/reasoner.hpp"
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <unordered_set>
namespace sima { namespace {
std::string key(ConceptId s,const std::string&r,ConceptId o){return std::to_string(s)+"#"+r+"#"+std::to_string(o);}
}
std::vector<Fact> Reasoner::infer(std::size_t cap)const{
 std::vector<Fact>out;std::unordered_set<std::string>known;for(const auto&f:knowledge_.facts)known.insert(key(f.subject,f.relation,f.object));
 for(const auto&rule:knowledge_.rules){if(!rule.transitive||out.size()>=cap)continue;
 std::unordered_map<ConceptId,std::vector<const Fact*>>adj;for(const auto&f:knowledge_.facts)if(f.relation==rule.relation)adj[f.subject].push_back(&f);
 std::vector<ConceptId>starts;starts.reserve(adj.size());for(const auto&[id,edges]:adj){(void)edges;starts.push_back(id);}
 for(auto start:starts){std::queue<ConceptId>q;std::unordered_set<ConceptId>seen; q.push(start);seen.insert(start);
 while(!q.empty()&&out.size()<cap){auto cur=q.front();q.pop();auto it=adj.find(cur);if(it==adj.end())continue;for(const auto*edge:it->second){auto target=edge->object;if(target==start||!seen.insert(target).second)continue;auto k=key(start,rule.relation,target);if(!known.contains(k)){known.insert(k);out.push_back({start,knowledge_.concepts.label(start),rule.relation,target,knowledge_.concepts.label(target),"inference: transitive",edge->confidence,FactOrigin::Inference,0});}q.push(target);if(out.size()>=cap)break;}}
 }}
 return out;
}
bool Reasoner::explain(ConceptId s,const std::string&r,ConceptId o,std::vector<Fact>&e)const{
 e.clear();for(const auto&f:knowledge_.facts)if(f.subject==s&&f.relation==r&&f.object==o){e.push_back(f);return true;}
 std::unordered_map<ConceptId,const Fact*>parent;std::queue<ConceptId>q;std::unordered_set<ConceptId>seen;q.push(s);seen.insert(s);
 while(!q.empty()){auto cur=q.front();q.pop();for(const auto&f:knowledge_.facts){if(f.subject!=cur||f.relation!=r)continue;if(seen.insert(f.object).second){parent[f.object]=&f;if(f.object==o){auto node=o;while(node!=s){auto*p=parent.at(node);e.push_back(*p);node=p->subject;}std::reverse(e.begin(),e.end());return true;}q.push(f.object);}}}
 return false;
}
}

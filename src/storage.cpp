#include "sima/storage.hpp"
#include <array>
#include <chrono>
#include <cmath>
#include <cstring>
#include <fstream>
#include <system_error>
namespace sima { namespace {
constexpr std::array<char,4> MAGIC{'S','I','M','A'}; constexpr std::uint32_t VERSION=1,MAX_STRING=16U*1024U*1024U; constexpr std::uint64_t MAX_ITEMS=10000000ULL;
void w32(std::ostream&o,std::uint32_t v){for(unsigned i=0;i<4;++i)o.put(static_cast<char>((v>>(i*8U))&255U));}
void w64(std::ostream&o,std::uint64_t v){for(unsigned i=0;i<8;++i)o.put(static_cast<char>((v>>(i*8U))&255U));}
bool r32(std::istream&i,std::uint32_t&v){v=0;for(unsigned n=0;n<4;++n){int c=i.get();if(c==EOF)return false;v|=static_cast<std::uint32_t>(static_cast<unsigned char>(c))<<(n*8U);}return true;}
bool r64(std::istream&i,std::uint64_t&v){v=0;for(unsigned n=0;n<8;++n){int c=i.get();if(c==EOF)return false;v|=static_cast<std::uint64_t>(static_cast<unsigned char>(c))<<(n*8U);}return true;}
void ws(std::ostream&o,const std::string&s){w32(o,static_cast<std::uint32_t>(s.size()));o.write(s.data(),static_cast<std::streamsize>(s.size()));}
bool rs(std::istream&i,std::string&s){std::uint32_t n=0;if(!r32(i,n)||n>MAX_STRING)return false;s.resize(n);return n==0||static_cast<bool>(i.read(s.data(),static_cast<std::streamsize>(n)));}
}
bool KnowledgeBase::add_fact(Fact f){if(!f.subject||!f.object||f.relation.empty()||!std::isfinite(f.confidence)||f.confidence<0||f.confidence>1||contains(f.subject,f.relation,f.object))return false;facts.push_back(std::move(f));rebuild_indexes();return true;}
bool KnowledgeBase::add_rule(Rule r){if(r.relation.empty())return false;for(const auto&x:rules)if(x.relation==r.relation&&x.transitive==r.transitive)return false;rules.push_back(std::move(r));return true;}
void KnowledgeBase::rebuild_indexes(){by_subject_.clear();for(std::size_t i=0;i<facts.size();++i)by_subject_.emplace(facts[i].subject,i);}
std::vector<const Fact*> KnowledgeBase::find(ConceptId s,const std::string&r)const{std::vector<const Fact*>v;auto range=by_subject_.equal_range(s);for(auto i=range.first;i!=range.second;++i){const auto&f=facts[i->second];if(f.relation==r)v.push_back(&f);}return v;}
bool KnowledgeBase::contains(ConceptId s,const std::string&r,ConceptId o)const{auto range=by_subject_.equal_range(s);for(auto i=range.first;i!=range.second;++i){const auto&f=facts[i->second];if(f.relation==r&&f.object==o)return true;}return false;}
KnowledgeStats KnowledgeBase::stats()const{return{concepts.size(),facts.size(),rules.size(),by_subject_.size()};}
void KnowledgeBase::clear(){concepts.clear();facts.clear();rules.clear();rebuild_indexes();}
bool KnowledgeBase::save(const std::filesystem::path&p,std::string&e)const{
 std::error_code ec;if(!p.parent_path().empty())std::filesystem::create_directories(p.parent_path(),ec);if(ec){e=ec.message();return false;}
 auto tmp=p;tmp+=std::string(".tmp.")+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
 {std::ofstream o(tmp,std::ios::binary|std::ios::trunc);if(!o){e="cannot open temporary file";return false;}
 o.write(MAGIC.data(),4);w32(o,VERSION);w64(o,concepts.all().size());
 for(const auto&[id,c]:concepts.all()){w64(o,id);ws(o,c.canonical);w32(o,static_cast<std::uint32_t>(c.aliases.size()));for(const auto&a:c.aliases)ws(o,a);}
 w64(o,facts.size());for(const auto&f:facts){w64(o,f.subject);ws(o,f.subject_label);ws(o,f.relation);w64(o,f.object);ws(o,f.object_label);ws(o,f.source);std::uint64_t bits=0;std::memcpy(&bits,&f.confidence,sizeof(bits));w64(o,bits);o.put(static_cast<char>(f.origin));w64(o,f.created_at);}
 w64(o,rules.size());for(const auto&r:rules){ws(o,r.relation);o.put(r.transitive?1:0);}o.flush();if(!o){e="write failed";o.close();std::filesystem::remove(tmp,ec);return false;}}
 std::filesystem::rename(tmp,p,ec);if(ec){e="cannot replace destination: "+ec.message();std::filesystem::remove(tmp,ec);return false;}return true;
}
bool KnowledgeBase::load(const std::filesystem::path&p,std::string&e){
 std::ifstream i(p,std::ios::binary);if(!i){e="cannot open file";return false;}std::array<char,4>m{};if(!i.read(m.data(),4)||m!=MAGIC){e="invalid SIMA signature";return false;}std::uint32_t ver=0;if(!r32(i,ver)||ver!=VERSION){e="unsupported storage version";return false;}
 KnowledgeBase c;std::uint64_t n=0;if(!r64(i,n)||n>MAX_ITEMS){e="invalid concept count";return false;}
 for(std::uint64_t x=0;x<n;++x){std::uint64_t id=0;std::string name;std::uint32_t ac=0;if(!r64(i,id)||!rs(i,name)||!r32(i,ac)||ac>100000){e="corrupt concept";return false;}std::vector<std::string>a;for(std::uint32_t z=0;z<ac;++z){std::string s;if(!rs(i,s)){e="corrupt alias";return false;}a.push_back(std::move(s));}if(!c.concepts.restore(id,name,a)){e="duplicate concept id";return false;}}
 if(!r64(i,n)||n>MAX_ITEMS){e="invalid fact count";return false;}for(std::uint64_t x=0;x<n;++x){Fact f;std::uint64_t bits=0;if(!r64(i,f.subject)||!rs(i,f.subject_label)||!rs(i,f.relation)||!r64(i,f.object)||!rs(i,f.object_label)||!rs(i,f.source)||!r64(i,bits)){e="corrupt fact";return false;}std::memcpy(&f.confidence,&bits,sizeof(bits));int origin=i.get();if(origin<0||origin>2||!r64(i,f.created_at)||!f.subject||!f.object||f.relation.empty()||!std::isfinite(f.confidence)||f.confidence<0||f.confidence>1){e="invalid fact";return false;}f.origin=static_cast<FactOrigin>(origin);c.facts.push_back(std::move(f));}
 if(!r64(i,n)||n>1000000){e="invalid rule count";return false;}for(std::uint64_t x=0;x<n;++x){Rule r;if(!rs(i,r.relation)){e="corrupt rule";return false;}int flag=i.get();if(flag<0||flag>1||r.relation.empty()){e="invalid rule";return false;}r.transitive=flag==1;c.rules.push_back(std::move(r));}
 if(i.peek()!=EOF){e="trailing data";return false;}c.rebuild_indexes();*this=std::move(c);return true;
}
}

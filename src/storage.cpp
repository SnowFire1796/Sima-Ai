#include "sima/storage.hpp"
#include "sima/text.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <system_error>
#include <utility>
namespace sima { namespace {
constexpr std::array<char,4> MAGIC{'S','I','M','A'};
constexpr std::uint32_t VERSION=2,MAX_STRING=16U*1024U*1024U;
constexpr std::uint64_t MAX_ITEMS=10000000ULL;
void write_u32(std::ostream&o,std::uint32_t v){for(unsigned i=0;i<4;++i)o.put(static_cast<char>((v>>(i*8U))&255U));}
void write_u64(std::ostream&o,std::uint64_t v){for(unsigned i=0;i<8;++i)o.put(static_cast<char>((v>>(i*8U))&255U));}
bool read_u32(std::istream&i,std::uint32_t&v){v=0;for(unsigned n=0;n<4;++n){int c=i.get();if(c==EOF)return false;v|=static_cast<std::uint32_t>(static_cast<unsigned char>(c))<<(n*8U);}return true;}
bool read_u64(std::istream&i,std::uint64_t&v){v=0;for(unsigned n=0;n<8;++n){int c=i.get();if(c==EOF)return false;v|=static_cast<std::uint64_t>(static_cast<unsigned char>(c))<<(n*8U);}return true;}
void write_string(std::ostream&o,const std::string&s){write_u32(o,static_cast<std::uint32_t>(s.size()));o.write(s.data(),static_cast<std::streamsize>(s.size()));}
bool read_string(std::istream&i,std::string&s){std::uint32_t n=0;if(!read_u32(i,n)||n>MAX_STRING)return false;s.resize(n);if(n&&!i.read(s.data(),static_cast<std::streamsize>(n)))return false;return text::is_valid_utf8(s);}
std::filesystem::path unique_sibling(const std::filesystem::path&p,const char*suffix){auto q=p;q+=std::string(suffix)+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());return q;}
std::string question_key(std::string_view question){const auto tokens=text::tokenize(question);std::string key;for(const auto&token:tokens){if(!key.empty())key.push_back(' ');key+=token;}return key;}
}
bool KnowledgeBase::add_fact(Fact f){if(!f.subject||!f.object||concepts.resolve(f.subject_label)!=f.subject||concepts.resolve(f.object_label)!=f.object||f.relation.empty()||!text::is_valid_utf8(f.relation)||!text::is_valid_utf8(f.source)||!std::isfinite(f.confidence)||f.confidence<0.0||f.confidence>1.0||static_cast<unsigned>(f.origin)>2U||contains(f.subject,f.relation,f.object))return false;const auto index=facts.size();facts.push_back(std::move(f));by_subject_.emplace(facts.back().subject,index);return true;}
bool KnowledgeBase::add_rule(Rule r){if(r.relation.empty()||!text::is_valid_utf8(r.relation))return false;for(const auto&x:rules)if(x.relation==r.relation&&x.transitive==r.transitive)return false;rules.push_back(std::move(r));return true;}
bool KnowledgeBase::add_response(ResponsePair response){const auto key=question_key(response.question);if(key.empty()||text::trim(response.answer).empty()||!text::is_valid_utf8(response.question)||!text::is_valid_utf8(response.answer)||!text::is_valid_utf8(response.source)||!std::isfinite(response.confidence)||response.confidence<0.0||response.confidence>1.0)return false;const auto range=responses_by_question_.equal_range(key);for(auto it=range.first;it!=range.second;++it){const auto&existing=responses[it->second];if(text::normalize(existing.answer)==text::normalize(response.answer))return false;}const auto index=responses.size();responses.push_back(std::move(response));responses_by_question_.emplace(key,index);return true;}
void KnowledgeBase::rebuild_indexes(){by_subject_.clear();responses_by_question_.clear();for(std::size_t i=0;i<facts.size();++i)by_subject_.emplace(facts[i].subject,i);for(std::size_t i=0;i<responses.size();++i)responses_by_question_.emplace(question_key(responses[i].question),i);}
std::vector<const Fact*>KnowledgeBase::find(ConceptId s,const std::string&r)const{std::vector<const Fact*>v;auto range=by_subject_.equal_range(s);for(auto i=range.first;i!=range.second;++i){const auto&f=facts[i->second];if(f.relation==r)v.push_back(&f);}return v;}
const ResponsePair*KnowledgeBase::find_response(std::string_view question)const{const auto key=question_key(question);if(key.empty())return nullptr;const auto range=responses_by_question_.equal_range(key);const ResponsePair*best=nullptr;for(auto it=range.first;it!=range.second;++it){const auto&candidate=responses[it->second];if(!best||candidate.confidence>best->confidence||(candidate.confidence==best->confidence&&candidate.answer<best->answer))best=&candidate;}return best;}
bool KnowledgeBase::contains(ConceptId s,const std::string&r,ConceptId o)const{auto range=by_subject_.equal_range(s);for(auto i=range.first;i!=range.second;++i){const auto&f=facts[i->second];if(f.relation==r&&f.object==o)return true;}return false;}
KnowledgeStats KnowledgeBase::stats()const{return{concepts.size(),facts.size(),rules.size(),responses.size(),by_subject_.size()};}
void KnowledgeBase::clear(){concepts.clear();facts.clear();rules.clear();responses.clear();rebuild_indexes();}

bool KnowledgeBase::save(const std::filesystem::path&p,std::string&e)const{
 std::error_code ec;if(!p.parent_path().empty())std::filesystem::create_directories(p.parent_path(),ec);if(ec){e="cannot create storage directory: "+ec.message();return false;}const auto tmp=unique_sibling(p,".tmp.");
 {
  std::ofstream o(tmp,std::ios::binary|std::ios::trunc);if(!o){e="cannot open temporary storage file";return false;}
  o.write(MAGIC.data(),4);write_u32(o,VERSION);std::vector<ConceptId>ids;ids.reserve(concepts.all().size());for(const auto&entry:concepts.all())ids.push_back(entry.first);std::sort(ids.begin(),ids.end());write_u64(o,ids.size());
  for(auto id:ids){const auto&c=concepts.all().at(id);write_u64(o,id);write_string(o,c.canonical);write_u32(o,static_cast<std::uint32_t>(c.aliases.size()));for(const auto&a:c.aliases)write_string(o,a);}
  write_u64(o,facts.size());for(const auto&f:facts){write_u64(o,f.subject);write_string(o,f.subject_label);write_string(o,f.relation);write_u64(o,f.object);write_string(o,f.object_label);write_string(o,f.source);std::uint64_t bits=0;static_assert(sizeof(bits)==sizeof(f.confidence));std::memcpy(&bits,&f.confidence,sizeof(bits));write_u64(o,bits);o.put(static_cast<char>(f.origin));write_u64(o,f.created_at);}
  write_u64(o,rules.size());for(const auto&r:rules){write_string(o,r.relation);o.put(r.transitive?1:0);}
  write_u64(o,responses.size());for(const auto&r:responses){write_string(o,r.question);write_string(o,r.answer);write_string(o,r.source);std::uint64_t bits=0;std::memcpy(&bits,&r.confidence,sizeof(bits));write_u64(o,bits);}
  o.flush();if(!o){e="failed while writing snapshot";o.close();std::filesystem::remove(tmp,ec);return false;}
 }
 std::filesystem::rename(tmp,p,ec);if(ec&&std::filesystem::exists(p)){const auto backup=unique_sibling(p,".bak.");std::error_code old_ec,new_ec;std::filesystem::rename(p,backup,old_ec);if(!old_ec){std::filesystem::rename(tmp,p,new_ec);if(new_ec){std::error_code rollback;std::filesystem::rename(backup,p,rollback);}else std::filesystem::remove(backup,old_ec);ec=new_ec;}}
 if(ec){e="cannot finalize snapshot: "+ec.message();std::filesystem::remove(tmp,ec);return false;}return true;
}

bool KnowledgeBase::load(const std::filesystem::path&p,std::string&e){
 std::ifstream in(p,std::ios::binary);if(!in){e="cannot open snapshot";return false;}std::array<char,4>magic{};if(!in.read(magic.data(),4)||magic!=MAGIC){e="invalid SIMA signature";return false;}
 std::uint32_t version=0;if(!read_u32(in,version)||(version!=1&&version!=VERSION)){e="unsupported storage version";return false;}
 KnowledgeBase candidate;std::uint64_t count=0;
 if(!read_u64(in,count)||count>MAX_ITEMS){e="invalid concept count";return false;}
 for(std::uint64_t x=0;x<count;++x){std::uint64_t id=0;std::string canonical;std::uint32_t aliases=0;if(!read_u64(in,id)||!read_string(in,canonical)||!read_u32(in,aliases)||aliases>100000U){e="corrupt concept record";return false;}std::vector<std::string>names;for(std::uint32_t j=0;j<aliases;++j){std::string a;if(!read_string(in,a)){e="corrupt alias";return false;}names.push_back(std::move(a));}if(!candidate.concepts.restore(id,canonical,names)){e="duplicate or invalid concept ID";return false;}}
 if(!read_u64(in,count)||count>MAX_ITEMS){e="invalid fact count";return false;}
 for(std::uint64_t x=0;x<count;++x){Fact f;std::uint64_t bits=0;if(!read_u64(in,f.subject)||!read_string(in,f.subject_label)||!read_string(in,f.relation)||!read_u64(in,f.object)||!read_string(in,f.object_label)||!read_string(in,f.source)||!read_u64(in,bits)){e="corrupt fact record";return false;}std::memcpy(&f.confidence,&bits,sizeof(bits));const int origin=in.get();if(origin<0||origin>2||!read_u64(in,f.created_at)||!std::isfinite(f.confidence)||f.confidence<0.0||f.confidence>1.0){e="invalid fact values";return false;}f.origin=static_cast<FactOrigin>(origin);if(!candidate.add_fact(std::move(f))){e="invalid or duplicate fact";return false;}}
 if(!read_u64(in,count)||count>1000000ULL){e="invalid rule count";return false;}
 for(std::uint64_t x=0;x<count;++x){Rule r;if(!read_string(in,r.relation)){e="corrupt rule record";return false;}const int flag=in.get();if(flag<0||flag>1||!candidate.add_rule({r.relation,flag==1})){e="invalid or duplicate rule";return false;}}
 if(version>=2){if(!read_u64(in,count)||count>MAX_ITEMS){e="invalid response count";return false;}for(std::uint64_t x=0;x<count;++x){ResponsePair r;std::uint64_t bits=0;if(!read_string(in,r.question)||!read_string(in,r.answer)||!read_string(in,r.source)||!read_u64(in,bits)){e="corrupt response record";return false;}std::memcpy(&r.confidence,&bits,sizeof(bits));if(!candidate.add_response(std::move(r))){e="invalid or duplicate response";return false;}}}
 if(in.peek()!=EOF){e="unexpected trailing bytes";return false;}candidate.rebuild_indexes();*this=std::move(candidate);return true;
}
}

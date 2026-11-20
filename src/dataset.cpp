#include "sima/dataset.hpp"
#include "sima/text.hpp"
#include <chrono>
#include <fstream>
#include <sstream>
namespace sima { namespace {
std::vector<std::string> split(const std::string&line){std::vector<std::string>v(1);bool esc=false;for(char c:line){if(esc){if(c=='|'||c=='\\')v.back().push_back(c);else{v.back().push_back('\\');v.back().push_back(c);}esc=false;}else if(c=='\\')esc=true;else if(c=='|')v.emplace_back();else v.back().push_back(c);}if(esc)v.back().push_back('\\');for(auto&s:v)s=text::trim(s);return v;}
std::uint64_t now(){return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());}
TrainReport read(std::istream&in,KnowledgeBase&kb,std::size_t max){TrainReport r;std::string line;while(std::getline(in,line)){++r.lines_read;if(line.size()>max){r.diagnostics.push_back({r.lines_read,"line exceeds size limit"});continue;}auto clean=text::trim(line);if(clean.empty()||clean[0]=='#')continue;auto f=split(clean);
if(f[0]=="FACT"){if((f.size()!=4&&f.size()!=5)||f[1].empty()||f[2].empty()||f[3].empty()){r.diagnostics.push_back({r.lines_read,"FACT requires subject|relation|object and optional source"});continue;}auto s=kb.concepts.ensure(f[1]),o=kb.concepts.ensure(f[3]);Fact fact{s,f[1],f[2],o,f[3],f.size()==5?f[4]:"dataset",1.0,FactOrigin::Dataset,now()};if(kb.add_fact(std::move(fact)))++r.facts_added;else ++r.duplicates;
}else if(f[0]=="WORD"){if(f.size()!=3||f[1].empty()||f[2].empty()){r.diagnostics.push_back({r.lines_read,"WORD requires alias|canonical"});continue;}if(kb.concepts.add_alias(f[1],f[2]))++r.aliases_added;else r.diagnostics.push_back({r.lines_read,"alias conflict or invalid alias"});
}else if(f[0]=="RULE"){if(f.size()!=3||f[1].empty()||f[2]!="TRANSITIVE"){r.diagnostics.push_back({r.lines_read,"expected RULE|relation|TRANSITIVE"});continue;}if(kb.add_rule({f[1],true}))++r.rules_added;else ++r.duplicates;
}else r.diagnostics.push_back({r.lines_read,"unknown record type: "+f[0]});}return r;}
}
TrainReport DatasetReader::train_file(const std::filesystem::path&p,KnowledgeBase&k,std::size_t m){std::ifstream in(p);if(!in){TrainReport r;r.diagnostics.push_back({0,"cannot open dataset: "+p.string()});return r;}return read(in,k,m);}
TrainReport DatasetReader::train_text(const std::string&s,KnowledgeBase&k,std::size_t m){std::istringstream in(s);return read(in,k,m);}
}

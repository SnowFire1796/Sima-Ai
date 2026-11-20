#include "sima/engine.hpp"
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
namespace {
constexpr const char*DEFAULT_FILE="storage/knowledge.sima";
std::vector<std::string>split(const std::string&s){std::istringstream in(s);std::vector<std::string>v;std::string x;while(in>>x)v.push_back(x);return v;}
void help(){std::cout<<"Sima AI 0.1 commands:\n train <file>\n ask <subject> <relation>\n query <text>\n why <subject> <relation> <object>\n list [limit]\n memory [limit]\n stats\n save [file]\n load [file]\n help\n exit\n";}
}
int main(){sima::SimaEngine engine;std::cout<<"Sima AI 0.1 — symbolic knowledge engine\nType help for commands.\n";std::string line;while(std::cout<<"sima> "&&std::getline(std::cin,line)){auto a=split(line);if(a.empty())continue;auto cmd=a[0];if(cmd=="exit"||cmd=="quit")break;if(cmd=="help"){help();continue;}
if(cmd=="train"){if(a.size()!=2){std::cout<<"Usage: train <file>\n";continue;}auto r=engine.train(a[1]);std::cout<<"Lines "<<r.lines_read<<"; facts +"<<r.facts_added<<"; aliases +"<<r.aliases_added<<"; rules +"<<r.rules_added<<"; duplicates "<<r.duplicates<<"\n";for(const auto&d:r.diagnostics)std::cout<<"line "<<d.line<<": "<<d.message<<"\n";}
else if(cmd=="ask"){if(a.size()!=3){std::cout<<"Usage: ask <subject> <relation>\n";continue;}auto v=engine.ask(a[1],a[2]);if(v.empty())std::cout<<"No direct fact found.\n";else for(auto*f:v)std::cout<<sima::format_fact(*f)<<"\n";}
else if(cmd=="query"){auto p=line.find_first_of(" \t");if(p==std::string::npos){std::cout<<"Usage: query <text>\n";continue;}std::cout<<engine.query(line.substr(p+1)).text<<"\n";}
else if(cmd=="why"){if(a.size()!=4){std::cout<<"Usage: why <subject> <relation> <object>\n";continue;}std::vector<sima::Fact>e;if(!engine.explain(a[1],a[2],a[3],e))std::cout<<"No supporting path found.\n";else std::cout<<sima::GenerationEngine{}.explain(e)<<"\n";}
else if(cmd=="list"){std::size_t n=50;if(a.size()>1){try{n=static_cast<std::size_t>(std::stoull(a[1]));}catch(...){std::cout<<"Invalid limit.\n";continue;}}const auto&v=engine.knowledge().facts;for(std::size_t i=0;i<v.size()&&i<n;++i)std::cout<<i+1<<". "<<sima::format_fact(v[i])<<"\n";std::cout<<"Displayed "<<std::min(n,v.size())<<" of "<<v.size()<<".\n";}
else if(cmd=="memory"){std::size_t n=10;if(a.size()>1){try{n=static_cast<std::size_t>(std::stoull(a[1]));}catch(...){std::cout<<"Invalid limit.\n";continue;}}for(const auto&m:engine.memory().recent(n))std::cout<<m.sequence<<". "<<m.input<<" => "<<m.response<<"\n";}
else if(cmd=="stats"){auto s=engine.stats();std::cout<<"Concepts: "<<s.concepts<<"\nFacts: "<<s.facts<<"\nRules: "<<s.rules<<"\nIndexed terms: "<<s.indexed_terms<<"\nSession memories: "<<engine.memory().size()<<"\n";}
else if(cmd=="save"||cmd=="load"){std::filesystem::path p=a.size()>1?a[1]:DEFAULT_FILE;std::string e;bool ok=cmd=="save"?engine.save(p,e):engine.load(p,e);std::cout<<(ok?(cmd=="save"?"Saved ":"Loaded "):"Error: ")<<(ok?p.string():e)<<"\n";}
else std::cout<<"Unknown command. Type help.\n";}
std::cout<<"Sima session ended.\n";return EXIT_SUCCESS;}

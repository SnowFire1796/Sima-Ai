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
std::vector<std::string>split(const std::string&line){std::istringstream input(line);std::vector<std::string>words;std::string word;while(input>>word)words.push_back(word);return words;}
void help(){std::cout<<"Sima AI 0.1 commands:\n train <file-or-directory>\n ask <subject> <relation>\n query <text>\n infer [limit]\n why <subject> <relation> <object>\n list [limit]\n memory [limit]\n recall <text>\n stats\n save [file]\n load [file]\n help\n exit\n";}
}
int main(){sima::SimaEngine engine;std::cout<<"Sima AI 0.1 — symbolic knowledge engine\nType help for commands.\n";std::string line;while(std::cout<<"sima> "&&std::getline(std::cin,line)){const auto args=split(line);if(args.empty())continue;const auto&command=args[0];if(command=="exit"||command=="quit")break;if(command=="help"){help();continue;}
if(command=="train"){if(args.size()!=2){std::cout<<"Usage: train <file-or-directory>\n";continue;}const auto report=engine.train(args[1]);std::cout<<"Lines "<<report.lines_read<<"; facts +"<<report.facts_added<<"; aliases +"<<report.aliases_added<<"; rules +"<<report.rules_added<<"; duplicates "<<report.duplicates<<"\n";for(const auto&d:report.diagnostics)std::cout<<"line "<<d.line<<": "<<d.message<<"\n";}
else if(command=="ask"){if(args.size()!=3){std::cout<<"Usage: ask <subject> <relation>\n";continue;}const auto facts=engine.ask(args[1],args[2]);if(facts.empty())std::cout<<"No direct fact found.\n";else for(const auto*fact:facts)std::cout<<sima::format_fact(*fact)<<"\n";}
else if(command=="query"){const auto pos=line.find_first_of(" \t");if(pos==std::string::npos){std::cout<<"Usage: query <text>\n";continue;}std::cout<<engine.query(line.substr(pos+1)).text<<"\n";}
else if(command=="infer"){std::size_t limit=10000;if(args.size()>1){try{limit=static_cast<std::size_t>(std::stoull(args[1]));}catch(...){std::cout<<"Invalid limit.\n";continue;}}const auto facts=engine.infer(limit);for(const auto&fact:facts)std::cout<<sima::format_fact(fact)<<"\n";std::cout<<"Derived "<<facts.size()<<" fact(s).\n";}
else if(command=="why"){if(args.size()!=4){std::cout<<"Usage: why <subject> <relation> <object>\n";continue;}std::vector<sima::Fact>evidence;if(!engine.explain(args[1],args[2],args[3],evidence))std::cout<<"No supporting path found.\n";else std::cout<<sima::GenerationEngine{}.explain(evidence)<<"\n";}
else if(command=="list"){std::size_t limit=50;if(args.size()>1){try{limit=static_cast<std::size_t>(std::stoull(args[1]));}catch(...){std::cout<<"Invalid limit.\n";continue;}}const auto&facts=engine.knowledge().facts;for(std::size_t i=0;i<facts.size()&&i<limit;++i)std::cout<<i+1<<". "<<sima::format_fact(facts[i])<<"\n";std::cout<<"Displayed "<<std::min(limit,facts.size())<<" of "<<facts.size()<<".\n";}
else if(command=="memory"||command=="recall"){if(command=="recall"){const auto pos=line.find_first_of(" \t");if(pos==std::string::npos){std::cout<<"Usage: recall <text>\n";continue;}for(const auto&entry:engine.memory().search(line.substr(pos+1)))std::cout<<entry.sequence<<". "<<entry.input<<" => "<<entry.response<<"\n";}else{std::size_t limit=10;if(args.size()>1){try{limit=static_cast<std::size_t>(std::stoull(args[1]));}catch(...){std::cout<<"Invalid limit.\n";continue;}}for(const auto&entry:engine.memory().recent(limit))std::cout<<entry.sequence<<". "<<entry.input<<" => "<<entry.response<<"\n";}}
else if(command=="stats"){const auto stats=engine.stats();std::cout<<"Concepts: "<<stats.concepts<<"\nFacts: "<<stats.facts<<"\nRules: "<<stats.rules<<"\nIndexed terms: "<<stats.indexed_terms<<"\nSession memories: "<<engine.memory().size()<<"\n";}
else if(command=="save"||command=="load"){const std::filesystem::path path=args.size()>1?args[1]:DEFAULT_FILE;std::string error;const bool ok=command=="save"?engine.save(path,error):engine.load(path,error);std::cout<<(ok?(command=="save"?"Saved ":"Loaded "):"Error: ")<<(ok?path.string():error)<<"\n";}
else std::cout<<"Unknown command. Type help.\n";}
std::cout<<"Sima session ended.\n";return EXIT_SUCCESS;}

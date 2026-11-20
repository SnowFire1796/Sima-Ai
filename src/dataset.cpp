#include "sima/dataset.hpp"
#include "sima/text.hpp"
#include <chrono>
#include <fstream>
#include <sstream>
#include <utility>
namespace sima { namespace {
std::vector<std::string> split_fields(const std::string&line){std::vector<std::string>fields(1);bool escaped=false;for(char c:line){if(escaped){if(c=='|'||c=='\\')fields.back().push_back(c);else{fields.back().push_back('\\');fields.back().push_back(c);}escaped=false;}else if(c=='\\')escaped=true;else if(c=='|')fields.emplace_back();else fields.back().push_back(c);}if(escaped)fields.back().push_back('\\');for(auto&f:fields)f=text::trim(f);return fields;}
std::uint64_t now_seconds(){return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());}
TrainReport process(std::istream&in,KnowledgeBase&kb,std::size_t max_line_bytes){TrainReport report;std::string line;while(std::getline(in,line)){++report.lines_read;if(line.size()>max_line_bytes){report.diagnostics.push_back({report.lines_read,"line exceeds configured byte limit"});continue;}if(!text::is_valid_utf8(line)){report.diagnostics.push_back({report.lines_read,"line is not valid UTF-8"});continue;}const auto clean=text::trim(line);if(clean.empty()||clean.front()=='#')continue;const auto fields=split_fields(clean);
if(fields[0]=="FACT"){if((fields.size()!=4&&fields.size()!=5)||fields[1].empty()||fields[2].empty()||fields[3].empty()){report.diagnostics.push_back({report.lines_read,"FACT requires subject|relation|object and optional source"});continue;}const auto subject=kb.concepts.ensure(fields[1]),object=kb.concepts.ensure(fields[3]);if(!subject||!object){report.diagnostics.push_back({report.lines_read,"unable to allocate concept identifier"});continue;}Fact fact{subject,fields[1],fields[2],object,fields[3],fields.size()==5?fields[4]:"dataset",1.0,FactOrigin::Dataset,now_seconds()};if(kb.add_fact(std::move(fact)))++report.facts_added;else ++report.duplicates;
}else if(fields[0]=="WORD"){if(fields.size()!=3||fields[1].empty()||fields[2].empty()){report.diagnostics.push_back({report.lines_read,"WORD requires alias|canonical"});continue;}if(kb.concepts.add_alias(fields[1],fields[2]))++report.aliases_added;else report.diagnostics.push_back({report.lines_read,"alias conflict or invalid alias"});
}else if(fields[0]=="RULE"){if(fields.size()!=3||fields[1].empty()||fields[2]!="TRANSITIVE"){report.diagnostics.push_back({report.lines_read,"expected RULE|relation|TRANSITIVE"});continue;}if(kb.add_rule({fields[1],true}))++report.rules_added;else ++report.duplicates;
}else if(fields[0]=="QA"){if((fields.size()!=3&&fields.size()!=4)||fields[1].empty()||fields[2].empty()){report.diagnostics.push_back({report.lines_read,"QA requires question|answer and optional source"});continue;}ResponsePair pair{fields[1],fields[2],fields.size()==4?fields[3]:"dataset",1.0};if(kb.add_response(std::move(pair)))++report.responses_added;else ++report.duplicates;
}else report.diagnostics.push_back({report.lines_read,"unknown record type: "+fields[0]});}return report;}
}
TrainReport DatasetReader::train_file(const std::filesystem::path&path,KnowledgeBase&knowledge,std::size_t max_line_bytes){std::ifstream in(path);if(!in){TrainReport r;r.diagnostics.push_back({0,"cannot open dataset: "+path.string()});return r;}return process(in,knowledge,max_line_bytes);}
TrainReport DatasetReader::train_text(const std::string&content,KnowledgeBase&knowledge,std::size_t max_line_bytes){std::istringstream in(content);return process(in,knowledge,max_line_bytes);}
}

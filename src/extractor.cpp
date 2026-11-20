#include "sima/extractor.hpp"
#include "sima/dataset.hpp"
#include "sima/text.hpp"
#include <chrono>
#include <fstream>
#include <sstream>
#include <utility>
namespace sima { namespace {
std::vector<std::string>sentences(const std::string&line){std::vector<std::string>out;std::size_t start=0;for(std::size_t i=0;i<line.size();++i){const auto c=static_cast<unsigned char>(line[i]);bool end=c=='.'||c=='!'||c=='?'||c==';';std::size_t width=1;if(i+1<line.size()&&c==0xD8U){const auto next=static_cast<unsigned char>(line[i+1]);if(next==0x9FU||next==0x9BU||next==0x8CU){end=true;width=2;}}if(end){auto part=text::trim(std::string_view(line).substr(start,i-start));if(!part.empty())out.push_back(std::move(part));i+=width-1;start=i+1;}}auto tail=text::trim(std::string_view(line).substr(start));if(!tail.empty())out.push_back(std::move(tail));return out;}
std::string join(const std::vector<std::string>&tokens,std::size_t first,std::size_t last){std::string value;for(std::size_t i=first;i<last;++i){if(!value.empty())value.push_back(' ');value+=tokens[i];}return value;}
bool article(const std::string&s){return s=="a"||s=="an"||s=="the";}
std::size_t skip_article(const std::vector<std::string>&tokens,std::size_t index){return index<tokens.size()&&article(tokens[index])?index+1:index;}
bool extract(const std::vector<std::string>&t,std::string&subject,std::string&relation,std::string&object){
 if(t.size()<3)return false;
 if(t.back()=="است"){
  subject=t.front();
  if(t.size()>=4&&t[1]=="یک"){relation="IsA";object=join(t,2,t.size()-1);return !object.empty();}
  if(t.size()>=4&&t[1]=="در"){relation="LocatedIn";object=join(t,2,t.size()-1);return !object.empty();}
  if(t.size()>=4&&t[1]=="دارای"){relation="Has";object=join(t,2,t.size()-1);return !object.empty();}
  if(t.size()>=5&&t[1]=="بخشی"&&t[2]=="از"){relation="PartOf";object=join(t,3,t.size()-1);return !object.empty();}
  if(t.size()==3){relation="IsA";object=t[1];return true;}
 }
 if(t.size()>=5&&t[1]=="به"&&t[2]=="دور"&&t.back()=="میچرخد"){subject=t.front();relation="Orbits";object=join(t,3,t.size()-1);return !object.empty();}
 std::size_t subject_index=0;if(article(t[subject_index])&&t.size()>3)subject_index=1;
 if(subject_index+2<t.size()){
  subject=t[subject_index];const auto verb=t[subject_index+1];
  if(verb=="orbits"||verb=="has"||verb=="contains"){relation=verb=="orbits"?"Orbits":(verb=="has"?"Has":"Contains");object=join(t,skip_article(t,subject_index+2),t.size());return !object.empty();}
  if(verb=="are"){relation="IsA";object=join(t,skip_article(t,subject_index+2),t.size());return !object.empty();}
  if(verb=="is"){
   std::size_t pos=subject_index+2;
   if(pos+1<t.size()&&t[pos]=="located"&&t[pos+1]=="in"){relation="LocatedIn";object=join(t,skip_article(t,pos+2),t.size());return !object.empty();}
   if(pos<t.size()&&t[pos]=="in"){relation="LocatedIn";object=join(t,skip_article(t,pos+1),t.size());return !object.empty();}
   if(pos+1<t.size()&&t[pos]=="part"&&t[pos+1]=="of"){relation="PartOf";object=join(t,skip_article(t,pos+2),t.size());return !object.empty();}
   pos=skip_article(t,pos);if(pos<t.size()){relation="IsA";object=join(t,pos,t.size());return !object.empty();}
  }
 }
 return false;
}
std::uint64_t timestamp(){return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());}
TrainReport process(std::istream&in,KnowledgeBase&kb,std::size_t max_bytes,const std::string&source){
 TrainReport report;std::string line;
 while(std::getline(in,line)){++report.lines_read;if(line.size()>max_bytes){report.diagnostics.push_back({report.lines_read,"line exceeds configured byte limit"});continue;}if(!text::is_valid_utf8(line)){report.diagnostics.push_back({report.lines_read,"line is not valid UTF-8"});continue;}const auto clean=text::trim(line);if(clean.empty()||clean.front()=='#')continue;
  if(clean.rfind("FACT|",0)==0||clean.rfind("WORD|",0)==0||clean.rfind("RULE|",0)==0){auto structured=DatasetReader::train_text(clean,kb,max_bytes);report.facts_added+=structured.facts_added;report.aliases_added+=structured.aliases_added;report.rules_added+=structured.rules_added;report.duplicates+=structured.duplicates;for(auto&d:structured.diagnostics){d.line=report.lines_read;report.diagnostics.push_back(std::move(d));}continue;}
  for(const auto&sentence:sentences(clean)){++report.sentences_seen;const auto tokens=text::tokenize(sentence);std::string subject,relation,object;if(!extract(tokens,subject,relation,object)){++report.sentences_unmatched;continue;}const auto sid=kb.concepts.ensure(subject),oid=kb.concepts.ensure(object);if(!sid||!oid){++report.sentences_unmatched;continue;}Fact fact{sid,subject,relation,oid,object,source+":"+std::to_string(report.lines_read),0.65,FactOrigin::Dataset,timestamp()};if(kb.add_fact(std::move(fact)))++report.facts_added;else ++report.duplicates;++report.sentences_extracted;}
 }
 return report;
}
}
TrainReport TextFactExtractor::train_file(const std::filesystem::path&path,KnowledgeBase&kb,std::size_t max){std::ifstream in(path);if(!in){TrainReport r;r.diagnostics.push_back({0,"cannot open text corpus: "+path.string()});return r;}return process(in,kb,max,path.string());}
TrainReport TextFactExtractor::train_text(const std::string&content,KnowledgeBase&kb,std::size_t max){std::istringstream in(content);return process(in,kb,max,"text");}
}

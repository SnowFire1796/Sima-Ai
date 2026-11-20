#include "sima/engine.hpp"
#include "sima/language_engine.hpp"
#include "sima/text.hpp"
#include <filesystem>
#include <iostream>
#include <stdexcept>
namespace {int checks=0;void check(bool ok,const char*message){++checks;if(!ok)throw std::runtime_error(message);}}
int main(){try{
 check(sima::text::normalize(" HELLO ")=="hello","ASCII normalization");
 check(sima::text::normalize("كي")=="کی","Persian character normalization");
 check(sima::text::detect_language("سلام")==sima::Language::Persian,"Persian detection");
 check(sima::text::detect_language("hello")==sima::Language::English,"English detection");
 check(sima::text::is_valid_utf8("سلام"),"valid UTF-8");
 check(!sima::text::is_valid_utf8(std::string(1,static_cast<char>(0xFF))),"invalid UTF-8");
 const auto parsed=sima::LanguageEngine{}.analyze("سلام world");check(parsed.language==sima::Language::Persian&&!parsed.tokens.empty(),"language pipeline");
 sima::SimaEngine engine;
 const auto report=engine.train_text("# sample\nFACT|Animal|IsA|LivingThing\nFACT|Dog|IsA|Animal\nWORD|سگ|Dog\nWORD|حیوان|Animal\nRULE|IsA|TRANSITIVE\n");
 check(report.facts_added==2,"fact ingestion");check(report.aliases_added==2,"alias ingestion");check(report.rules_added==1,"rule ingestion");
 check(engine.knowledge().concepts.resolve("سگ")==engine.knowledge().concepts.resolve("Dog"),"shared cross-language concept");
 check(engine.ask("سگ","IsA").size()==1,"alias lookup");
 const auto derived=engine.infer();check(derived.size()==1&&derived.front().object_label=="LivingThing","transitive inference output");
 std::vector<sima::Fact>path;check(engine.explain("Dog","IsA","LivingThing",path),"transitive explanation");check(path.size()==2,"explanation path length");
 const auto answer=engine.query("Dog Animal");check(answer.sufficient&&!answer.evidence.empty(),"evidence-backed retrieval");check(engine.memory().size()==1,"session memory records query");
 sima::MemoryEngine memory(2);memory.remember("first question","first answer");memory.remember("second question","second answer");memory.remember("third question","third answer");check(memory.size()==2,"memory capacity bound");check(memory.recent(1).front().input=="third question","recent memory ordering");check(memory.search("third").size()==1,"memory lexical recall");
 sima::KnowledgeBase escaped;const auto escaped_report=sima::DatasetReader::train_text("FACT|A\\|B|RelatedTo|C\n",escaped);check(escaped_report.facts_added==1,"escaped separator parsed");check(escaped.concepts.resolve("A|B")!=0,"escaped field preserved");
 sima::KnowledgeBase no_rule;auto a=no_rule.concepts.ensure("A"),b=no_rule.concepts.ensure("B"),c=no_rule.concepts.ensure("C");no_rule.add_fact({a,"A","Link",b,"B","test",1.0,sima::FactOrigin::Dataset,0});no_rule.add_fact({b,"B","Link",c,"C","test",1.0,sima::FactOrigin::Dataset,0});std::vector<sima::Fact>unsupported;check(!sima::Reasoner(no_rule).explain(a,"Link",c,unsupported),"no implicit transitivity without rule");
 auto file=std::filesystem::temp_directory_path()/"sima-test.sima";std::string error;check(engine.save(file,error),"binary save");sima::SimaEngine restored;check(restored.load(file,error),"binary load");check(restored.stats().facts==2,"round-trip fact count");check(restored.knowledge().concepts.resolve("سگ")!=0,"round-trip aliases");check(!restored.load(file.string()+".missing",error),"failed load rejected");check(restored.stats().facts==2,"failed load preserves active knowledge");std::filesystem::remove(file);
 std::cout<<"All "<<checks<<" checks passed.\n";return 0;
 }catch(const std::exception&e){std::cerr<<"TEST FAILURE: "<<e.what()<<"\n";return 1;}}

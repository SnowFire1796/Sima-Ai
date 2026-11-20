#include "sima/engine.hpp"
#include "sima/extractor.hpp"
#include "sima/language_engine.hpp"
#include "sima/text.hpp"
#include <cstdint>
#include <filesystem>
#include <fstream>
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
 check(sima::text::tokenize("پایتخت ایران کجاست؟").size()==3,"Persian punctuation tokenization");
 const auto parsed=sima::LanguageEngine{}.analyze("سلام world");check(parsed.language==sima::Language::Persian&&!parsed.tokens.empty(),"language pipeline");
 sima::SimaEngine engine;
 const auto report=engine.train_text("# sample\nFACT|Animal|IsA|LivingThing\nFACT|Dog|IsA|Animal\nWORD|سگ|Dog\nWORD|حیوان|Animal\nRULE|IsA|TRANSITIVE\n");
 check(report.facts_added==2,"fact ingestion");check(report.aliases_added==2,"alias ingestion");check(report.rules_added==1,"rule ingestion");
 check(engine.knowledge().concepts.resolve("سگ")==engine.knowledge().concepts.resolve("Dog"),"shared cross-language concept");
 check(engine.ask("سگ","IsA").size()==1,"alias lookup");
 const auto duplicate=engine.train_text("FACT|Dog|IsA|Animal\n");check(duplicate.duplicates==1&&engine.stats().facts==2,"duplicate fact protection");
 const auto conflict=engine.train_text("WORD|سگ|Cat\n");check(!conflict.diagnostics.empty()&&engine.knowledge().concepts.resolve("Cat")==0,"alias collision does not create orphan concept");
 const auto qa_report=engine.train_text("QA|What is the capital of France?|Paris|world-atlas\nQA|پایتخت ایران کجاست؟|تهران|atlas-fa\n");check(qa_report.responses_added==2&&engine.stats().responses==2,"QA pair ingestion");
 const auto duplicate_qa=engine.train_text("QA|what is the capital of France|Paris|duplicate-source\n");check(duplicate_qa.duplicates==1,"normalized duplicate QA rejected");
 const auto exact=engine.query("WHAT IS THE CAPITAL OF FRANCE?");check(exact.sufficient&&exact.text=="Paris"&&exact.source=="world-atlas","normalized exact QA retrieval");
 const auto persian_exact=engine.query("پایتخت ایران کجاست؟");check(persian_exact.sufficient&&persian_exact.text=="تهران","Persian QA punctuation normalization");
 const auto derived=engine.infer();check(derived.size()==1&&derived.front().object_label=="LivingThing","transitive inference output");check(engine.infer(0).empty(),"inference output cap");
 std::vector<sima::Fact>path;check(engine.explain("Dog","IsA","LivingThing",path),"transitive explanation");check(path.size()==2,"explanation path length");
 const auto answer=engine.query("Dog Animal");check(answer.sufficient&&!answer.evidence.empty(),"evidence-backed retrieval");check(engine.memory().size()==3,"session memory records answers");check(!engine.query("qzxvnonexistent").sufficient,"unsupported query remains uncertain");
 sima::MemoryEngine memory(2);memory.remember("first question","first answer");memory.remember("second question","second answer");memory.remember("third question","third answer");check(memory.size()==2,"memory capacity bound");check(memory.recent(1).front().input=="third question","recent memory ordering");check(memory.search("third").size()==1,"memory lexical recall");
 sima::KnowledgeBase escaped;const auto escaped_report=sima::DatasetReader::train_text("FACT|A\\|B|RelatedTo|C\n",escaped);check(escaped_report.facts_added==1,"escaped separator parsed");check(escaped.concepts.resolve("A|B")!=0,"escaped field preserved");
 const std::string prose="Earth is a planet. The Sun is a star. Earth orbits the Sun. Water is in the ocean.\nزمین یک سیاره است. زمین به دور خورشید می‌چرخد.\n";sima::KnowledgeBase extracted;const auto extraction=sima::TextFactExtractor::train_text(prose,extracted);check(extraction.sentences_seen==6&&extraction.sentences_extracted==6,"bilingual text templates extracted");check(extracted.contains(extracted.concepts.resolve("Earth"),"IsA",extracted.concepts.resolve("planet")),"English prose fact");check(extracted.contains(extracted.concepts.resolve("Earth"),"Orbits",extracted.concepts.resolve("sun")),"English article normalization");check(extracted.contains(extracted.concepts.resolve("water"),"LocatedIn",extracted.concepts.resolve("ocean")),"English location template");check(extracted.contains(extracted.concepts.resolve("زمین"),"Orbits",extracted.concepts.resolve("خورشید")),"Persian orbit fact");
 std::string invalid_line="FACT|";invalid_line.push_back(static_cast<char>(0xFF));invalid_line+="|RelatedTo|C\n";sima::KnowledgeBase invalid;const auto invalid_report=sima::DatasetReader::train_text(invalid_line,invalid);check(invalid_report.facts_added==0&&!invalid_report.diagnostics.empty(),"invalid UTF-8 dataset line rejected");
 sima::KnowledgeBase no_rule;const auto a=no_rule.concepts.ensure("A"),b=no_rule.concepts.ensure("B"),c=no_rule.concepts.ensure("C");no_rule.add_fact({a,"A","Link",b,"B","test",1.0,sima::FactOrigin::Dataset,0});no_rule.add_fact({b,"B","Link",c,"C","test",1.0,sima::FactOrigin::Dataset,0});std::vector<sima::Fact>unsupported;check(!sima::Reasoner(no_rule).explain(a,"Link",c,unsupported),"no implicit transitivity without rule");
 const auto folder=std::filesystem::temp_directory_path()/"sima-dataset-test";std::filesystem::create_directories(folder);{std::ofstream sample(folder/"batch.sima");sample<<"FACT|BatchSubject|ConnectedTo|BatchObject\nQA|What is two plus two?|Four|math\n";}{std::ofstream prose_file(folder/"prose.txt");prose_file<<"Mercury is a planet.\n";}sima::KnowledgeBase batch;const auto batch_report=sima::LearningEngine{}.train_path(folder,batch);check(batch_report.facts_added==2&&batch_report.responses_added==1&&batch_report.sentences_extracted==1,"directory training mixes facts, QA, and prose datasets");std::filesystem::remove_all(folder);
 const auto file=std::filesystem::temp_directory_path()/"sima-test.sima";std::string error;check(engine.save(file,error),"binary save");check(engine.save(file,error),"replace existing binary snapshot");sima::SimaEngine restored;check(restored.load(file,error),"binary load");check(restored.stats().facts==2,"round-trip fact count");check(restored.stats().responses==2,"round-trip QA count");check(restored.knowledge().find_response("what is the capital of France?")!=nullptr,"round-trip exact response index");check(restored.knowledge().concepts.resolve("سگ")!=0,"round-trip aliases");check(!restored.load(file.string()+".missing",error),"failed load rejected");check(restored.stats().facts==2&&restored.stats().responses==2,"failed load preserves active knowledge");std::filesystem::remove(file);
 const auto legacy=std::filesystem::temp_directory_path()/"sima-v1-test.sima";{std::ofstream out(legacy,std::ios::binary);out.write("SIMA",4);auto put32=[&](std::uint32_t value){for(unsigned i=0;i<4;++i)out.put(static_cast<char>((value>>(i*8U))&255U));};auto put64=[&](std::uint64_t value){for(unsigned i=0;i<8;++i)out.put(static_cast<char>((value>>(i*8U))&255U));};put32(1);put64(0);put64(0);put64(0);}sima::SimaEngine legacy_engine;check(legacy_engine.load(legacy,error)&&legacy_engine.stats().responses==0,"version 1 snapshot compatibility");std::filesystem::remove(legacy);
 std::cout<<"All "<<checks<<" checks passed.\n";return 0;
 }catch(const std::exception&e){std::cerr<<"TEST FAILURE: "<<e.what()<<"\n";return 1;}}

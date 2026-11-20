#include "sima/engine.hpp"
#include "sima/text.hpp"
#include <filesystem>
#include <iostream>
#include <stdexcept>
namespace {int checks=0;void check(bool ok,const char*m){++checks;if(!ok)throw std::runtime_error(m);}}
int main(){try{
check(sima::text::normalize(" HELLO ")=="hello","ASCII normalization");
check(sima::text::normalize("كي")=="کی","Persian character normalization");
check(sima::text::detect_language("سلام")==sima::Language::Persian,"Persian detection");
check(sima::text::detect_language("hello")==sima::Language::English,"English detection");
check(sima::text::is_valid_utf8("سلام"),"valid UTF-8");
check(!sima::text::is_valid_utf8(std::string(1,static_cast<char>(0xFF))),"invalid UTF-8");
sima::SimaEngine e;auto r=e.train_text("# demo\nFACT|Animal|IsA|LivingThing\nFACT|Dog|IsA|Animal\nWORD|سگ|Dog\nWORD|حیوان|Animal\nRULE|IsA|TRANSITIVE\n");
check(r.facts_added==2,"facts ingested");check(r.aliases_added==2,"aliases ingested");check(r.rules_added==1,"rule ingested");
check(e.knowledge().concepts.resolve("سگ")==e.knowledge().concepts.resolve("Dog"),"shared concept alias");
check(e.ask("سگ","IsA").size()==1,"alias lookup");
std::vector<sima::Fact>path;check(e.explain("Dog","IsA","LivingThing",path),"transitive explanation");check(path.size()==2,"explanation path length");
auto q=e.query("Dog Animal");check(q.sufficient&&!q.evidence.empty(),"retrieval");
auto file=std::filesystem::temp_directory_path()/"sima-test.sima";std::string error;check(e.save(file,error),"save");sima::SimaEngine restored;check(restored.load(file,error),"load");check(restored.stats().facts==2,"roundtrip facts");check(restored.knowledge().concepts.resolve("سگ")!=0,"roundtrip alias");std::filesystem::remove(file);
std::cout<<"All "<<checks<<" checks passed.\n";return 0;
}catch(const std::exception&x){std::cerr<<"TEST FAILURE: "<<x.what()<<"\n";return 1;}}

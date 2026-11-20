#include "sima/learning.hpp"
#include "sima/extractor.hpp"
#include <algorithm>
#include <system_error>
#include <utility>
namespace sima { namespace {
void merge(TrainReport&target,TrainReport source,const std::string&file){target.lines_read+=source.lines_read;target.facts_added+=source.facts_added;target.aliases_added+=source.aliases_added;target.rules_added+=source.rules_added;target.responses_added+=source.responses_added;target.duplicates+=source.duplicates;target.sentences_seen+=source.sentences_seen;target.sentences_extracted+=source.sentences_extracted;target.sentences_unmatched+=source.sentences_unmatched;for(auto&diagnostic:source.diagnostics){diagnostic.message=file+": "+diagnostic.message;target.diagnostics.push_back(std::move(diagnostic));}}
bool accepted(const std::filesystem::path&path){const auto extension=path.extension().string();return extension==".sima"||extension==".txt";}
TrainReport train_one(const std::filesystem::path&path,KnowledgeBase&knowledge,std::size_t max_line_bytes){return path.extension()==".txt"?TextFactExtractor::train_file(path,knowledge,max_line_bytes):DatasetReader::train_file(path,knowledge,max_line_bytes);}
}
TrainReport LearningEngine::train_path(const std::filesystem::path&path,KnowledgeBase&knowledge,bool recursive)const{
 TrainReport total;std::error_code ec;const bool regular=std::filesystem::is_regular_file(path,ec);if(ec){total.diagnostics.push_back({0,"cannot inspect dataset path: "+ec.message()});return total;}if(regular){if(accepted(path))merge(total,train_one(path,knowledge,max_line_bytes_),path.string());else total.diagnostics.push_back({0,"unsupported dataset extension: "+path.string()});return total;}
 const bool directory=std::filesystem::is_directory(path,ec);if(ec||!directory){total.diagnostics.push_back({0,"dataset path is not a readable file or directory: "+path.string()});return total;}
 std::vector<std::filesystem::path>files;auto collect=[&](const std::filesystem::directory_entry&entry){std::error_code status_error;const bool is_file=entry.is_regular_file(status_error);if(status_error){total.diagnostics.push_back({0,"cannot inspect "+entry.path().string()+": "+status_error.message()});return;}if(is_file&&accepted(entry.path()))files.push_back(entry.path());};
 if(recursive){std::filesystem::recursive_directory_iterator it(path,std::filesystem::directory_options::skip_permission_denied,ec),end;if(ec){total.diagnostics.push_back({0,"cannot open dataset directory: "+ec.message()});return total;}while(it!=end){collect(*it);it.increment(ec);if(ec){total.diagnostics.push_back({0,"directory traversal stopped: "+ec.message()});break;}}}
 else{std::filesystem::directory_iterator it(path,std::filesystem::directory_options::skip_permission_denied,ec),end;if(ec){total.diagnostics.push_back({0,"cannot open dataset directory: "+ec.message()});return total;}while(it!=end){collect(*it);it.increment(ec);if(ec){total.diagnostics.push_back({0,"directory traversal stopped: "+ec.message()});break;}}}
 std::sort(files.begin(),files.end(),[](const auto&a,const auto&b){return a.generic_string()<b.generic_string();});for(const auto&file:files)merge(total,train_one(file,knowledge,max_line_bytes_),file.string());return total;
}
}

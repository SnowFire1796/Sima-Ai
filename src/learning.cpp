#include "sima/learning.hpp"
#include <system_error>
#include <utility>
namespace sima { namespace {
void merge(TrainReport&target,TrainReport source,const std::string&file){target.lines_read+=source.lines_read;target.facts_added+=source.facts_added;target.aliases_added+=source.aliases_added;target.rules_added+=source.rules_added;target.duplicates+=source.duplicates;for(auto&d:source.diagnostics){d.message=file+": "+d.message;target.diagnostics.push_back(std::move(d));}}
bool accepted(const std::filesystem::path&path){const auto ext=path.extension().string();return ext==".sima"||ext==".txt";}
}
TrainReport LearningEngine::train_path(const std::filesystem::path&path,KnowledgeBase&knowledge,bool recursive)const{
 TrainReport total;std::error_code ec;
 if(std::filesystem::is_regular_file(path,ec)){if(ec){total.diagnostics.push_back({0,"cannot inspect dataset path: "+ec.message()});return total;}if(accepted(path))merge(total,DatasetReader::train_file(path,knowledge,max_line_bytes_),path.string());else total.diagnostics.push_back({0,"unsupported dataset extension: "+path.string()});return total;}
 ec.clear();if(!std::filesystem::is_directory(path,ec)||ec){total.diagnostics.push_back({0,"dataset path is not a readable file or directory: "+path.string()});return total;}
 auto add_file=[&](const std::filesystem::directory_entry&entry){std::error_code status_error;if(entry.is_regular_file(status_error)&&!status_error&&accepted(entry.path()))merge(total,DatasetReader::train_file(entry.path(),knowledge,max_line_bytes_),entry.path().string());};
 if(recursive){std::filesystem::recursive_directory_iterator it(path,std::filesystem::directory_options::skip_permission_denied,ec),end;while(it!=end){if(ec){total.diagnostics.push_back({0,"directory traversal error: "+ec.message()});ec.clear();it.increment(ec);continue;}add_file(*it);it.increment(ec);}}
 else{std::filesystem::directory_iterator it(path,std::filesystem::directory_options::skip_permission_denied,ec),end;while(it!=end){if(ec){total.diagnostics.push_back({0,"directory traversal error: "+ec.message()});ec.clear();it.increment(ec);continue;}add_file(*it);it.increment(ec);}}
 return total;
}
}

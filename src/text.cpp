#include "sima/text.hpp"
#include <cctype>
#include <cstdint>
namespace sima::text { namespace {
bool decode(std::string_view s,std::size_t& i,std::uint32_t& cp){
 if(i>=s.size())return false; auto c=static_cast<unsigned char>(s[i]); if(c<128){cp=c;++i;return true;}
 std::size_t n=0; if((c&0xE0U)==0xC0U){cp=c&0x1FU;n=1;}else if((c&0xF0U)==0xE0U){cp=c&0x0FU;n=2;}else if((c&0xF8U)==0xF0U){cp=c&7U;n=3;}else return false;
 if(i+n>=s.size())return false; for(std::size_t j=1;j<=n;++j){auto d=static_cast<unsigned char>(s[i+j]);if((d&0xC0U)!=0x80U)return false;cp=(cp<<6U)|(d&0x3FU);}
 if((n==1&&cp<128)||(n==2&&cp<2048)||(n==3&&cp<65536)||cp>0x10FFFF||(cp>=0xD800&&cp<=0xDFFF))return false;i+=n+1;return true;
}
void append(std::string& o,std::uint32_t c){if(c<128)o.push_back(static_cast<char>(c));else if(c<2048){o.push_back(static_cast<char>(0xC0U|(c>>6U)));o.push_back(static_cast<char>(0x80U|(c&63U)));}else if(c<65536){o.push_back(static_cast<char>(0xE0U|(c>>12U)));o.push_back(static_cast<char>(0x80U|((c>>6U)&63U)));o.push_back(static_cast<char>(0x80U|(c&63U)));}else{o.push_back(static_cast<char>(0xF0U|(c>>18U)));o.push_back(static_cast<char>(0x80U|((c>>12U)&63U)));o.push_back(static_cast<char>(0x80U|((c>>6U)&63U)));o.push_back(static_cast<char>(0x80U|(c&63U)));}}
}
std::string trim(std::string_view s){std::size_t a=0,b=s.size();while(a<b&&std::isspace(static_cast<unsigned char>(s[a])))++a;while(b>a&&std::isspace(static_cast<unsigned char>(s[b-1])))--b;return std::string(s.substr(a,b-a));}
bool is_valid_utf8(std::string_view s){std::size_t i=0;std::uint32_t cp=0;while(i<s.size())if(!decode(s,i,cp))return false;return true;}
std::string normalize(std::string_view s){std::string o;o.reserve(s.size());std::size_t i=0;while(i<s.size()){auto p=i;std::uint32_t c=0;if(!decode(s,i,c)){o.push_back(s[p]);i=p+1;continue;}if(c==0x064A||c==0x0649)c=0x06CC;if(c==0x0643)c=0x06A9;if(c==0x0622||c==0x0623||c==0x0625)c=0x0627;if(c==0x200C||c==0x200D||c==0x0640)continue;if(c<128)o.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));else append(o,c);}return trim(o);}
std::vector<std::string> tokenize(std::string_view s){auto n=normalize(s);std::vector<std::string> v;std::string t;for(unsigned char c:n){if(std::isspace(c)||c==','||c=='.'||c=='?'||c=='!'||c==':'||c==';'||c=='('||c==')'){if(!t.empty()){v.push_back(std::move(t));t.clear();}}else t.push_back(static_cast<char>(c));}if(!t.empty())v.push_back(std::move(t));return v;}
Language detect_language(std::string_view s){std::size_t i=0;std::uint32_t c=0;while(i<s.size()){if(!decode(s,i,c))continue;if((c>=0x0600&&c<=0x06FF)||(c>=0x0750&&c<=0x077F)||(c>=0x08A0&&c<=0x08FF))return Language::Persian;if((c>='A'&&c<='Z')||(c>='a'&&c<='z'))return Language::English;}return Language::Unknown;}
}

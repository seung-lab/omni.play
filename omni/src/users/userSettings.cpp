
#include "users/userSettings.h"

namespace om {

void userSettings::Load()
{
    using namespace YAML;
    
    if(om::file::exists(filename_))
    {
        Node in;
        om::yaml::yamlUtil::Read(filename_, in);
        
        om::yaml::yamlUtil::OptionalRead(in, "threshold", threshold_, defaultThreshold_);
        om::yaml::yamlUtil::OptionalRead(in, "sizeThreshold", sizeThreshold_, defaultSizeThreshold_);
    }
}

void userSettings::Save()
{
    using namespace YAML;
    
    Emitter out;
    
    out << BeginDoc << BeginMap;
    out << Key << "threshold" << Value << threshold_;
    out << Key << "sizeThreshold" << Value << sizeThreshold_;
    out << EndMap << EndDoc;
    
    om::yaml::yamlUtil::Write(filename_, out);
}


} // namespace om
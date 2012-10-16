
#include "users/userSettings.h"

namespace om {

void userSettings::Load()
{
    using namespace YAML;

    if(om::file::exists(filename_))
    {
        Node in;
        om::yaml::util::Read(filename_, in);

        om::yaml::util::OptionalRead(in, "threshold", threshold_, defaultThreshold_);
        om::yaml::util::OptionalRead(in, "sizeThreshold", sizeThreshold_, defaultSizeThreshold_);
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

    om::yaml::util::Write(filename_, out);
}


} // namespace om
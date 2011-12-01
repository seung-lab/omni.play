#pragma once


#include "yaml-cpp/yaml.h"

namespace om {

namespace proj {
class projectImpl;
class volumes;
}

namespace datalayer {
namespace archive {

static const int Latest_Project_Version = 25;

class project {
public:
    static void Read(const std::string& fnp, proj::projectImpl* project);
    static void Write(const std::string& fnp, proj::projectImpl* project);
private:
    static void postLoad();
};

} // namespace archive
} // namespace datalayer
} // namespace om

namespace YAML {

YAML::Emitter &operator<<(YAML::Emitter& out, const om::proj::projectImpl& p);
void operator>>(const YAML::Node& in, om::proj::projectImpl& p);
YAML::Emitter &operator<<(YAML::Emitter& out, const om::proj::volumes& p);
void operator>>(const YAML::Node& in, om::proj::volumes& p);

} // namespace YAML

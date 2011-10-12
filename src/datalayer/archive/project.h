#pragma once


#include "yaml-cpp/yaml.h"

class projectImpl;
class OmPreferences;
class projectVolumes;

namespace om {
namespace datalayer {
namespace archive {

static const int Latest_Project_Version = 25;

class project {
public:
    static void Read(const std::string& fnp, projectImpl* project);
    static void Write(const std::string& fnp, projectImpl* project);
private:
    static void postLoad();    
};

} // namespace archive
} // namespace datalayer
} // namespace om

namespace YAML {
    
YAML::Emitter &operator<<(YAML::Emitter& out, const projectImpl& p);
void operator>>(const YAML::Node& in, projectImpl& p);
YAML::Emitter &operator<<(YAML::Emitter& out, const OmPreferences& p);
void operator>>(const YAML::Node& in, OmPreferences& p);
YAML::Emitter &operator<<(YAML::Emitter& out, const projectVolumes& p);
void operator>>(const YAML::Node& in, projectVolumes& p);

} // namespace YAML

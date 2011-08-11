#pragma once

#include <QString>
#include "yaml-cpp/yaml.h"

class OmProjectImpl;
class OmPreferences;
class OmProjectVolumes;

namespace om {
namespace data {
namespace archive {

static const int Latest_Project_Version = 25;

class project {
public:
    static void Read(const QString& fnp, OmProjectImpl* project);
    static void Write(const QString& fnp, OmProjectImpl* project);
private:
    static void postLoad();    
};

YAML::Emitter &operator<<(YAML::Emitter& out, const OmProjectImpl& p);
void operator>>(const YAML::Node& in, OmProjectImpl& p);
YAML::Emitter &operator<<(YAML::Emitter& out, const OmPreferences& p);
void operator>>(const YAML::Node& in, OmPreferences& p);
YAML::Emitter &operator<<(YAML::Emitter& out, const OmProjectVolumes& p);
void operator>>(const YAML::Node& in, OmProjectVolumes& p);

}; // namespace archive
}; // namespace data
}; // namespace om
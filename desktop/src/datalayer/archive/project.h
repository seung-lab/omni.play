#pragma once
#include "precomp.h"

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

}  // namespace archive
}  // namespace data
}  // namespace om

namespace YAMLold {

YAMLold::Emitter& operator<<(YAMLold::Emitter& out, const OmProjectImpl& p);
void operator>>(const YAMLold::Node& in, OmProjectImpl& p);
YAMLold::Emitter& operator<<(YAMLold::Emitter& out, const OmPreferences& p);
void operator>>(const YAMLold::Node& in, OmPreferences& p);
YAMLold::Emitter& operator<<(YAMLold::Emitter& out, const OmProjectVolumes& p);
void operator>>(const YAMLold::Node& in, OmProjectVolumes& p);

}  // namespace YAMLold

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

namespace YAML {

template <>
struct convert<OmProjectImpl> {
  static Node encode(const OmProjectImpl&);
  static bool decode(const Node& node, OmProjectImpl&);
};
template <>
struct convert<OmPreferences> {
  static Node encode(const OmPreferences&);
  static bool decode(const Node& node, OmPreferences&);
};
template <>
struct convert<OmProjectVolumes> {
  static Node encode(const OmProjectVolumes&);
  static bool decode(const Node& node, OmProjectVolumes&);
};

}  // namespace YAML

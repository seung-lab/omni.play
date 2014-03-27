#include "annotation/annotation.h"
#include "annotation/annotationYaml.hpp"
#include "volume/omSegmentation.h"
#include "utility/yaml/omYaml.hpp"
#include "utility/yaml/manager.hpp"
#include "utility/yaml/coords.h"
#include "project/omProject.h"
#include "project/omProjectGlobals.h"
#include "users/omUsers.h"

namespace om {
namespace annotation {

std::string manager::filePathV1() const {
  return OmProject::Globals()
      .Users()
      .UserPaths()
      .Annotations(vol_->GetID())
      .string();
}

void manager::Add(coords::Global coord, const std::string& comment,
                  const common::Color& color, double size) {
  base_t::Add(new data(coord.ToData(vol_->Coords(), 0), comment, color, size));
  event::AnnotationObjectModified();
  event::Redraw2d();
  event::Redraw3d();
}

void manager::Save() const {
  std::string fnp = filePathV1();

  YAMLold::Emitter e;

  e << YAMLold::BeginDoc;
  base_t::Save(e);
  e << YAMLold::EndDoc;

  yaml::util::Write(fnp, e);
}

data* manager::parse(const YAMLold::Node& n) {
  coords::Global c;
  n["coord"] >> c;
  std::string comment;
  n["comment"] >> comment;
  common::Color color;
  n["color"] >> color;
  double size;
  yaml::util::OptionalRead(n, "size", size, 3.0);
  return new data(c.ToData(vol_->Coords(), 0), comment, color, size);
}

void manager::Load() {
  std::string fnp = filePathV1();
  if (!file::old::exists(fnp)) {
    log_infos << "Unable to find Annotations file.  Will create new one.";
    return;
  }

  YAMLold::Node n;
  try {
    yaml::util::Read(fnp, n);
    base_t::Load(n);
  }
  catch (YAMLold::Exception e) {
    std::stringstream ss;
    ss << "Error Loading Annotations: " << e.what() << ".\n";
    throw IoException(ss.str());
  }

  return;
}
}
}  // namespace annotation::

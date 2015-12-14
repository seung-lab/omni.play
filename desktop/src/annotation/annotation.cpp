#include "annotation/annotation.h"
#include "annotation/annotationYaml.hpp"
#include "volume/omSegmentation.h"
#include "utility/yaml/omYaml.hpp"
#include "utility/yaml/manager.hpp"
#include "coordinates/yaml.hpp"
#include "project/omProject.h"
#include "project/omProjectGlobals.h"
#include "users/omUsers.h"
#include "datalayer/file.h"

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

  YAML::Node n;
  base_t::Save(n);


  if (!file::exists(fnp)) {
    log_infos << "Creating new Annotations file. " << fnp;
    auto file = om::file::path(fnp);
    om::file::MkDir(file.parent_path());
  }

  try {
    std::ofstream file(fnp);
    log_debugs << "saving anotation to " << fnp;
    if (n.size() == 0){
      file << "";
    } else {
      file << n;
    }
  }
  catch (std::exception& e) {
    log_errors << "Error writing Annotation file: " << e.what();
  }
}

data* manager::parse(const YAML::Node& n) {
  auto c = n["coord"].as<coords::Global>();
  auto comment = n["comment"].as<std::string>();
  auto color = n["color"].as<common::Color>();
  auto size = n["size"].as<size_t>(3.0);
  return new data(c.ToData(vol_->Coords(), 0), comment, color, size);
}

void manager::Load() {
  std::string fnp = filePathV1();

  if (!file::exists(fnp)) {
    return;
  }

  try {
    YAML::Node n = YAML::LoadFile(fnp);
    base_t::Load(n);
  }
  catch (YAML::Exception& e) {
    std::stringstream ss;
    ss << "Error Loading Annotations: " << e.what() << ".\n";
    throw IoException(ss.str());
  }

  return;
}
}
}  // namespace annotation::

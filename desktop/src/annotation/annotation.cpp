#include "coordinates/dataCoord.h"
#include "coordinates/globalCoord.h"
#include "annotation/annotation.h"
#include "annotation/annotationYaml.hpp"
#include "volume/omSegmentation.h"
#include "volume/omSegmentationFolder.h"
#include "utility/yaml/yaml.hpp"
#include "utility/yaml/manager.hpp"

#include <fstream>
#include <sstream>

namespace om {
namespace annotation {

std::string manager::filePathV1() const {
  return vol_->Folder()->AnnotationFile().toStdString();
}

void manager::Add(globalCoord coord, const std::string& comment,
                  const om::common::Color& color, double size) {
  base_t::Add(new data(coord.toDataCoord(vol_, 0), comment, color, size));
  OmEvents::AnnotationEvent();
  OmEvents::Redraw2d();
  OmEvents::Redraw3d();
}

void manager::Save() const {
  std::string fnp = filePathV1();

  YAML::Emitter e;

  e << YAML::BeginDoc;
  base_t::Save(e);
  e << YAML::EndDoc;

  om::yaml::Util::Write(fnp, e);
}

data* manager::parse(const YAML::Node& n) {
  globalCoord c;
  n["coord"] >> c;
  auto comment = n["comment"].as<std::string>();
  auto color = n["color"].as<om::common::Color>();
  auto size = n["size"].as<double>(3.0);
  return new data(c.toDataCoord(vol_, 0), comment, color, size);
}

void manager::Load() {
  std::string fnp = filePathV1();
  if (!om::file::old::exists(fnp)) {
    std::cout << "Unable to find Annotations file.  Creating new one."
              << std::endl;
    return;
  }

  YAML::Node n;
  try {
    om::yaml::Util::Read(fnp, n);
    base_t::Load(n);
  }
  catch (YAML::Exception e) {
    std::stringstream ss;
    ss << "Error Loading Annotations: " << e.what() << ".\n";
    throw om::IoException(ss.str());
  }

  return;
}

}
}  // namespace om::annotation::

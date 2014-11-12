#include "datalayer/archive/project.h"
#include "datalayer/archive/channel.h"
#include "datalayer/archive/segmentation.h"
#include "common/common.h"
#include "utility/omFileHelpers.h"
#include "datalayer/file.h"
#include "project/omProject.h"
#include "system/omPreferences.h"
#include "project/omProjectImpl.hpp"
#include "utility/yaml/omBaseTypes.hpp"
#include "utility/channelDataWrapper.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "utility/segmentDataWrapper.hpp"
#include "volume/omFilter2d.h"
#include "mesh/omMeshManagers.hpp"
#include "utility/yaml/baseTypes.hpp"

namespace om {
namespace data {
namespace archive {

void project::Read(const QString& fnp, OmProjectImpl* project) {
  using namespace YAML;

  try {
    auto docs = YAML::LoadAllFromFile(fnp.toStdString());

    auto ver = docs[0]["version"].as<int>();
    OmProject::setFileVersion(ver);

    YAML::convert<OmProjectImpl>::decode(docs[1], *project);
  }
  catch (Exception e) {
    /*
            std::stringstream ss;
            ss << e.what() << "\n";
            ss << fnp.toStdString();
            ss << " line: " << e.mark.line;
            ss << " col: " << e.mark.column;
            ss << " pos: " << e.mark.pos;
            throw om::IoException(ss.str());
    */
    throw;
  }
  postLoad();
}

void project::Write(const QString& fnp, OmProjectImpl* project) {
  using namespace YAML;

  const QString fnpOld = fnp + ".old";

  try {
    if (OmFileHelpers::DoesFileExist(fnp)) {
      OmFileHelpers::MoveFile(fnp, fnpOld);
    }
  }
  catch (...) {
  }

  OmProject::setFileVersion(Latest_Project_Version);

  std::ofstream fout(fnp.toLatin1());
  YAML::Emitter e(fout);
  e << YAML::BeginDoc << YAML::Node(Latest_Project_Version) << YAML::EndDoc;
  e << YAML::BeginDoc << YAML::convert<OmProjectImpl>::encode(*project)
    << YAML::EndDoc;
}

void project::postLoad() {
  FOR_EACH(iter, ChannelDataWrapper::ValidIDs()) {
    const ChannelDataWrapper cdw(*iter);

    if (cdw.IsBuilt()) {
      std::vector<OmFilter2d*> filters = cdw.GetFilters();

      FOR_EACH(fiter, filters) {
        OmFilter2d* filter = *fiter;
        filter->Load();
      }
    }
  }

  for (auto& seg : SegmentationDataWrapper::GetPtrVec()) {
    if (seg->built()) {
      seg->MeshManagers().Load();
    }
  }
}

}  // namespace archive
}  // namespace data
}  // namespace om

namespace YAML {
Node convert<OmProjectImpl>::encode(const OmProjectImpl& p) {
  Node n;
  n["Preferences"] = OmPreferences::instance();
  n["Volumes"] = p.volumes_;
  return n;
}

bool convert<OmProjectImpl>::decode(const Node& node, OmProjectImpl& p) {
  if (!node.IsMap()) {
    return false;
  }
  YAML::convert<OmPreferences>::decode(node["Preferences"],
                                       OmPreferences::instance());
  YAML::convert<OmProjectVolumes>::decode(node["Volumes"], p.volumes_);
  return true;
}

Node convert<OmPreferences>::encode(const OmPreferences& p) {
  Node n;
  n["String Preferences"] = p.stringPrefs_;
  n["Float Preferences"] = p.floatPrefs_;
  n["Int Preferences"] = p.intPrefs_;
  n["Bool Preferences"] = p.boolPrefs_;
  n["V3f Preferences"] = p.v3fPrefs_;
  return n;
}
bool convert<OmPreferences>::decode(const Node& node, OmPreferences& p) {
  if (!node.IsMap()) {
    return false;
  }

  YAML::convert<QHash<int, QString>>::decode(node["String Preferences"],
                                             p.stringPrefs_);
  YAML::convert<QHash<int, float>>::decode(node["Float Preferences"],
                                           p.floatPrefs_);
  YAML::convert<QHash<int, int>>::decode(node["Int Preferences"], p.intPrefs_);
  YAML::convert<QHash<int, bool>>::decode(node["Bool Preferences"],
                                          p.boolPrefs_);
  YAML::convert<QHash<int, Vector3f>>::decode(node["V3f Preferences"],
                                              p.v3fPrefs_);
  return true;
}

Node convert<OmProjectVolumes>::encode(const OmProjectVolumes& p) {
  Node n;
  n["Channels"] = *p.channels_;
  n["Segmentations"] = *p.segmentations_;
  return n;
}

bool convert<OmProjectVolumes>::decode(const Node& node, OmProjectVolumes& p) {
  if (!node.IsMap()) {
    return false;
  }
  YAML::convert<OmChannelManager>::decode(node["Channels"], *p.channels_);
  YAML::convert<OmSegmentationManager>::decode(node["Segmentations"],
                                               *p.segmentations_);
  return true;
}

}  // namespace YAML

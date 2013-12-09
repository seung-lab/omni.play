#pragma once

#include "segment/io/omMSTtypes.h"
#include "zi/omMutex.h"
#include "datalayer/archive/segmentation.h"
#include "project/omProject.h"
#include "project/omProjectGlobals.h"
#include "users/omUsers.h"

#include <QString>

class OmSegmentation;
template <class> class OmIOnDiskFile;
template <class> class OmMemMappedFileReadQT;
template <class> class OmMemMappedFileWriteQT;
template <class> class OmFileReadQT;
template <class> class OmFileWriteQT;
class QDataStream;

class OmMST {
 public:
  OmMST(OmSegmentation* segmentation);

  ~OmMST() {}

  static double DefaultThreshold;

  void Read();
  void Flush();

  void Import(const std::vector<OmMSTImportEdge>& edges);

  inline bool IsValid() const { return numEdges_ > 0; }

  inline uint32_t NumEdges() const { return numEdges_; }

  inline double UserThreshold() const {
    return OmProject::Globals().Users().UserSettings().getThreshold();
  }

  inline double UserSizeThreshold() const {
    return OmProject::Globals().Users().UserSettings().getSizeThreshold();
  }

  void SetUserThreshold(const double t);
  void SetUserSizeThreshold(const double t);

  inline OmMSTEdge* Edges() { return edges_; }

 private:
  OmSegmentation* const vol_;

  uint32_t numEdges_;

  zi::rwmutex thresholdLock_;

  std::shared_ptr<OmIOnDiskFile<OmMSTEdge> > edgesPtr_;
  OmMSTEdge* edges_;

  void create();
  void convert();

  // read MST via full load into memory
  typedef OmFileReadQT<OmMSTEdge> reader_t;
  typedef OmFileWriteQT<OmMSTEdge> writer_t;

  // read MST via mem map
  // typedef OmMemMappedFileReadQT<OmMSTEdge> reader_t;
  // typedef OmMemMappedFileWriteQT<OmMSTEdge> writer_t;

  std::string filePathActual();

  friend class SegmentTests1;

  friend class OmDataArchiveProjectImpl;
  friend QDataStream& operator<<(QDataStream& out, const OmSegmentation& seg);
  friend YAMLold::Emitter& YAMLold::operator<<(YAMLold::Emitter& out,
                                               const OmSegmentation& seg);
  friend void YAMLold::operator>>(const YAMLold::Node& in, OmSegmentation& seg);
};

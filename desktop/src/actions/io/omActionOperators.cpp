#include "actions/io/omActionOperators.h"
#include "datalayer/archive/old/omDataArchiveBoost.h"
#include "segment/omSegment.h"
#include "utility/dataWrappers.h"
#include "utility/map.hpp"
#include "volume/omSegmentation.h"
#include "datalayer/archive/old/omDataArchiveWrappers.h"
#include "datalayer/archive/segmentEdge.hpp"

#include <boost/serialization/map.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "actions/details/omSegmentJoinActionImpl.hpp"
#include "actions/details/omSegmentSelectActionImpl.hpp"
#include "actions/details/omSegmentSplitActionImpl.hpp"
#include "actions/details/omSegmentShatterActionImpl.hpp"
#include "actions/details/omSegmentCutActionImpl.hpp"
#include "actions/details/omSegmentValidateActionImpl.hpp"
#include "actions/details/omSegmentUncertainActionImpl.hpp"
#include "actions/details/omSegmentationThresholdChangeActionImpl.hpp"
#include "actions/details/omAutomaticSpreadingThresholdChangeActionImpl.hpp"
#include "actions/details/omSegmentationSizeThresholdChangeActionImpl.hpp"
#include "actions/details/omVoxelSetValueActionImpl.hpp"
#include "actions/details/omProjectCloseActionImpl.hpp"

QDataStream& operator<<(QDataStream& out,
                        const OmSegmentValidateActionImpl& a) {
  int version = 1;
  out << version;

  om::common::SegIDSet ids;
  std::set<OmSegment*>* segs = a.selectedSegments_.get();
  FOR_EACH(iter, *segs) {
    OmSegment* seg = *iter;
    ids.insert(seg->value());
  }
  out << ids;
  out << a.valid_;
  out << a.sdw_.GetSegmentationID();

  return out;
}

QDataStream& operator>>(QDataStream& in, OmSegmentValidateActionImpl& a) {
  int version;
  in >> version;

  om::common::SegIDSet ids;
  in >> ids;
  in >> a.valid_;

  in >> a.sdw_;

  OmSegments* cache = a.sdw_.Segments();

  std::shared_ptr<std::set<OmSegment*> > segs =
      std::make_shared<std::set<OmSegment*> >();

  FOR_EACH(iter, ids) {
    OmSegment* seg = cache->GetSegment(*iter);
    segs->insert(seg);
  }

  a.selectedSegments_ = segs;

  return in;
}

QDataStream& operator<<(QDataStream& out,
                        const OmSegmentUncertainActionImpl& a) {
  int version = 1;
  out << version;

  om::common::SegIDSet ids;
  std::set<OmSegment*>* segs = a.selectedSegments_.get();
  FOR_EACH(iter, *segs) {
    OmSegment* seg = *iter;
    ids.insert(seg->value());
  }
  out << ids;
  out << a.uncertain_;
  out << a.sdw_;

  return out;
}

QDataStream& operator>>(QDataStream& in, OmSegmentUncertainActionImpl& a) {
  int version;
  in >> version;

  om::common::SegIDSet ids;
  in >> ids;
  in >> a.uncertain_;

  in >> a.sdw_;

  OmSegments* cache = a.sdw_.Segments();

  std::shared_ptr<std::set<OmSegment*> > segs =
      std::make_shared<std::set<OmSegment*> >();

  FOR_EACH(iter, ids) {
    OmSegment* seg = cache->GetSegment(*iter);
    segs->insert(seg);
  }

  a.selectedSegments_ = segs;

  return in;
}

QDataStream& operator<<(QDataStream& out, const OmSegmentSplitActionImpl& a) {
  int version = 3;
  out << version;
  out << a.mEdge;
  out << a.mSegmentationID;
  out << a.desc;

  return out;
}

template <class T>
QDataStream& operator<<(QDataStream& out, const Vector3<T>& v) {
  out << v.array[0];
  out << v.array[1];
  out << v.array[2];
  return out;
}

template <class T>
QDataStream& operator>>(QDataStream& in, Vector3<T>& v) {
  in >> v.array[0];
  in >> v.array[1];
  in >> v.array[2];
  return in;
}

QDataStream& operator>>(QDataStream& in, OmSegmentSplitActionImpl& a) {
  int version;
  in >> version;

  in >> a.mEdge;
  in >> a.mSegmentationID;
  in >> a.desc;

  if (2 == version) {
    om::common::SegID deadSeg;
    in >> deadSeg;

    Vector3i dc;
    in >> dc;
    in >> dc;
  }

  return in;
}

QDataStream& operator<<(QDataStream& out, const OmSegmentShatterActionImpl& a) {
  int version = 1;
  out << version;

  out << a.sdw_;
  out << a.desc;

  return out;
}

QDataStream& operator>>(QDataStream& in, OmSegmentShatterActionImpl& a) {
  int version;
  in >> version;

  in >> a.sdw_;
  in >> a.desc;

  return in;
}

QDataStream& operator<<(QDataStream& out, const OmSegmentJoinActionImpl& a) {
  int version = 1;
  out << version;
  out << a.sdw_;
  out << a.segIDs_;

  return out;
}

QDataStream& operator>>(QDataStream& in, OmSegmentJoinActionImpl& a) {
  int version;
  in >> version;
  in >> a.sdw_;
  in >> a.segIDs_;

  return in;
}

QDataStream& operator<<(QDataStream& out, const OmSegmentSelectActionImpl& a) {
  int version = 3;
  out << version;
  out << a.params_->sdw.GetSegmentationID();
  // QT map only allows std map, must convert!
  out << QMap<om::common::SegID, uint32_t>(
      om::map::ToStdMap(a.params_->newSelectedIDs));
  out << QMap<om::common::SegID, uint32_t>(
      om::map::ToStdMap(a.params_->oldSelectedIDs));
  out << a.params_->sdw.GetSegmentID();

  return out;
}

QDataStream& operator>>(QDataStream& in, OmSegmentSelectActionImpl& a) {
  int version;
  in >> version;

  std::shared_ptr<OmSelectSegmentsParams> params =
      std::make_shared<OmSelectSegmentsParams>();

  om::common::ID segmentationID;
  in >> segmentationID;

  QMap<om::common::SegID, uint32_t> newSelectedIDs;
  QMap<om::common::SegID, uint32_t> oldSelectedIDs;

  in >> newSelectedIDs;
  in >> oldSelectedIDs;

  // QT map only allows std map, must convert!
  params->newSelectedIDs = om::map::ToUnorderedMap(newSelectedIDs.toStdMap());
  params->oldSelectedIDs = om::map::ToUnorderedMap(oldSelectedIDs.toStdMap());

  om::common::SegID segmentID;
  in >> segmentID;

  params->sdw = SegmentDataWrapper(segmentationID, segmentID);

  a.params_ = params;

  return in;
}

QDataStream& operator<<(QDataStream& out, const OmProjectSaveActionImpl&) {
  int version = 1;
  out << version;

  return out;
}

QDataStream& operator>>(QDataStream& in, OmProjectSaveActionImpl&) {
  int version;
  in >> version;

  return in;
}

QDataStream& operator<<(QDataStream& out, const OmProjectCloseActionImpl&) {
  int version = 1;
  out << version;

  return out;
}

QDataStream& operator>>(QDataStream& in, OmProjectCloseActionImpl&) {
  int version;
  in >> version;

  return in;
}

QDataStream& operator<<(QDataStream& out,
                        const OmSegmentationThresholdChangeActionImpl& a) {
  int version = 2;
  out << version;
  out << a.threshold_;
  out << a.oldThreshold_;
  out << a.sdw_;

  return out;
}

QDataStream& operator>>(QDataStream& in,
                        OmSegmentationThresholdChangeActionImpl& a) {
  int version;
  in >> version;
  in >> a.threshold_;
  in >> a.oldThreshold_;

  om::common::ID id = 1;

  if (version > 1) {
    in >> id;
  } else {
    log_infos << "WARNGING: guessing segmentation ID...";
  }

  a.sdw_ = SegmentationDataWrapper(id);

  return in;
}

QDataStream& operator<<(QDataStream& out,
                        const OmSegmentationSizeThresholdChangeActionImpl& a) {
  int version = 2;
  out << version;
  out << a.threshold_;
  out << a.oldThreshold_;
  out << a.sdw_;

  return out;
}

QDataStream& operator>>(QDataStream& in,
                        OmSegmentationSizeThresholdChangeActionImpl& a) {
  int version;
  in >> version;
  in >> a.threshold_;
  in >> a.oldThreshold_;

  om::common::ID id = 1;

  if (version > 1) {
    in >> id;
  } else {
    log_infos << "WARNGING: guessing segmentation ID...";
  }

  a.sdw_ = SegmentationDataWrapper(id);

    return in;
}

QDataStream& operator<<(QDataStream& out, const OmAutomaticSpreadingThresholdChangeActionImpl& a)
{
    int version = 2;
    out << version;
    out << a.threshold_;
    out << a.oldThreshold_;
    out << a.sdw_;

    return out;
}

QDataStream& operator>>(QDataStream& in, OmAutomaticSpreadingThresholdChangeActionImpl& a)
{
    int version;
    in >> version;
    in >> a.threshold_;
    in >> a.oldThreshold_;

    om::common::ID id = 1;

    if(version > 1){
        in >> id;
    } else {
        printf("WARNGING: guessing segmentation ID...\n");
    }

    a.sdw_ = SegmentationDataWrapper(id);

  return in;
}

QDataStream& operator<<(QDataStream& out, const OmVoxelSetValueActionImpl& a) {
  int version = 1;
  out << version;
  out << a.segmentationID_;
  // out << a.mOldVoxelValues;  //FIXME
  out << a.newSegmentID_;

  return out;
}

QDataStream& operator>>(QDataStream& in, OmVoxelSetValueActionImpl& a) {
  int version;
  in >> version;
  in >> a.segmentationID_;
  // in >> a.mOldVoxelValues; //FIXME
  in >> a.newSegmentID_;

  return in;
}

QDataStream& operator<<(QDataStream& out, const OmSegmentCutActionImpl& a) {
  int version = 1;
  out << version;

  out << a.sdw_;
  out << a.edges_;

  return out;
}

QDataStream& operator>>(QDataStream& in, OmSegmentCutActionImpl& a) {
  int version;
  in >> version;

  in >> a.sdw_;
  in >> a.edges_;

  return in;
}

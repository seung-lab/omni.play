#pragma once
#include "precomp.h"

#include "segment/io/omValidGroupNum.hpp"
#include "segment/lists/omSegmentLists.h"
#include "segment/omSegment.h"
#include "segment/omSegments.h"
#include "segment/omSegmentIterator.h"
#include "segment/omSegmentSearched.hpp"
#include "segment/omSegmentSelected.hpp"
#include "utility/color.hpp"
#include "utility/segmentationDataWrapper.hpp"

class OmSegmentUtils {
 public:
  std::shared_ptr<std::set<OmSegment*>> static GetAllChildrenSegments(
      const OmSegments& segments, const om::common::SegIDSet& set) {
    OmSegmentIterator iter(segments);
    iter.iterOverSegmentIDs(set);

    OmSegment* seg = iter.getNextSegment();

    auto* children = new std::set<OmSegment*>();
    while (nullptr != seg) {
      children->insert(seg);
      seg = iter.getNextSegment();
    }

    return std::shared_ptr<std::set<OmSegment*>>(children);
  }

  template <class A, class B>
  static void GetAllChildrenSegments(const SegmentationDataWrapper& sdw,
                                     const A& segsIn, B& ret) {
    OmSegmentIterator iter(sdw);
    iter.iterOverSegmentIDs(segsIn);

    OmSegment* seg = iter.getNextSegment();

    while (nullptr != seg) {
      ret.push_back(seg);
      seg = iter.getNextSegment();
    }
  }

  template <class B>
  static void GetAllChildrenSegments(const OmSegments& segments,
                                     const om::common::SegID segID, B& ret) {
    OmSegmentIterator iter(segments);
    iter.iterOverSegmentID(segID);

    OmSegment* seg = iter.getNextSegment();

    while (nullptr != seg) {
      ret.push_back(seg);
      seg = iter.getNextSegment();
    }
  }

  static OmSegment* GetSegmentFromThreshold(OmSegment* seg,
                                            const float breakThreshold) {
    // WARNING: operation is O(depth of MST)

    OmSegment* segWalker = seg;

    OmSegment* parent = segWalker->getParent();
    float segThreshold = segWalker->getThreshold();

    while (parent && segThreshold > breakThreshold && segThreshold < 2) {
      segWalker = parent;
      segThreshold = segWalker->getThreshold();
      parent = segWalker->getParent();
    }

    return segWalker;
  }

  std::shared_ptr<std::vector<std::string>> static GetChildrenInfo(
      const SegmentDataWrapper& sdw) {
    auto ret = std::make_shared<std::vector<std::string>>();

    if (!sdw.IsSegmentValid()) {
      return ret;
    }

    OmSegmentIterator iter(sdw);
    iter.iterOverSegmentID(sdw.FindRootID());

    OmSegment* seg = iter.getNextSegment();

    while (nullptr != seg) {
      OmSegment* parent = seg->getParent();

      auto parentID = parent ? parent->value() : 0;

      const QString str = QString("%1 : %2, %3, %4")
                              .arg(seg->value())
                              .arg(parentID)
                              .arg(seg->getThreshold())
                              .arg(seg->size());

      std::string line = str.toStdString();
      ret->push_back(line);

      seg = iter.getNextSegment();
    }

    return ret;
  }

  static void ReValidateEveryObject(const SegmentationDataWrapper& sdw) {
    OmSegments* s = sdw.Segments();
    if (!s) {
      log_debugs << "Unable to ReValidateEveryObject.  Invalid "
                    "SegmentationDataWrapper.";
      return;
    }
    OmSegments& segments = *s;

    OmValidGroupNum* validGroupNum = sdw.ValidGroupNum();

    for (auto i = 1; i <= segments.maxValue(); ++i) {
      OmSegment* seg = segments.GetSegment(i);

      // only process valid, root (i.e. parent == 0) segments
      if (!seg || !seg->IsValidListType() || seg->getParent()) {
        continue;
      }

      std::deque<OmSegment*> children;
      OmSegmentUtils::GetAllChildrenSegments(segments, i, children);

      validGroupNum->Set(children, true);
    }
  }

  static om::common::SegID GetNextSegIDinWorkingList(
      const SegmentDataWrapper& sdw) {
    return GetNextSegIDinWorkingList(sdw.MakeSegmentationDataWrapper());
  }

  static om::common::SegID GetNextSegIDinWorkingList(
      const SegmentationDataWrapper& sdw) {
    return sdw.SegmentLists()->GetNextSegIDinWorkingList(sdw);
  }

  static om::common::SegID GetFirstSegIDInWorkingList(
      const SegmentDataWrapper& sdw) {
    return sdw.SegmentLists()->GetFirstSegIDInWorkingList(sdw.MakeSegmentationDataWrapper());
  }

  static om::common::SegID GetFirstSegIDInWorkingList(
      const SegmentationDataWrapper& sdw) {
    return sdw.SegmentLists()->GetFirstSegIDInWorkingList(sdw);
  }

  static std::string ListTypeAsStr(const om::common::SegListType type) {
    switch (type) {
      case om::common::SegListType::WORKING:
        return "working";
      case om::common::SegListType::VALID:
        return "valid";
      case om::common::SegListType::UNCERTAIN:
        return "uncertain";
      default:
        return "unknown type";
    }
  }

  static QColor SetSegColor(const SegmentDataWrapper& sdw,
                            const QColor& color) {
    // max allowed color val is 128 to allow color to be highlighted
    // const om::common::Color c = { std::min(128, color.red()),
    //                     std::min(128, color.green()),
    //                     std::min(128, color.blue()) };

    const om::common::Color c = {static_cast<uint8_t>(color.red()),
                                 static_cast<uint8_t>(color.green()),
                                 static_cast<uint8_t>(color.blue())};

    sdw.SetColor(c);

    return om::utils::color::ColorToQColor(c);
  }

  static QColor SegColorAsQColor(const SegmentDataWrapper& sdw) {
    return om::utils::color::ColorToQColor(sdw.GetColorInt());
  }

  template <class A, class B>
  static void GetSegPtrs(const SegmentationDataWrapper& sdw, const A& ids,
                         B& segPtrs) {
    OmSegments* cache = sdw.Segments();

    for (auto& id : ids) {
      OmSegment* seg = cache->GetSegment(id);

      if (!seg) {
        continue;
      }

      segPtrs.push_back(seg);
    }
  }
};

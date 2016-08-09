#pragma once
#include "precomp.h"

#include "common/common.h"
#include "events/events.h"
#include "project/omProject.h"
#include "segment/lists/omSegmentLists.h"
#include "segment/omSegments.h"
#include "system/cache/omCacheManager.h"
#include "utility/dataWrappers.h"
#include "viewGroup/omViewGroupState.h"
#include "segment/boostgraph/boostGraph.hpp"
#include <functional>

class OmSegmentMultiSplitActionImpl {
 private:
  std::vector<om::segment::UserEdge> splitEdges_;
  std::vector<om::segment::UserEdge> joinEdges_;
  om::common::SegIDSet sources_;
  om::common::SegIDSet sinks_;
  om::common::ID segmentationID_;

  /*
   * Yo Dawg! Heard you like functions, so we put your function inside this function
   * so you can operate while you operate!
   *
   * (Used in for_each)
   * 
   * Parameters:
   *  edgeOperator: A function that allows you to input an edge and operate on it
   *    (i.e. splitting or merging) and returning another edge
   * Returns:
   *  a void function with parameter edge that retrieves a new edge from edgeOperator.
   *  parameter edge is updated with the new edge returned by the operator
   * 
   */
  std::function<void(om::segment::UserEdge&)> 
    updateEdgeWithAction(
        std::function<om::segment::UserEdge(om::segment::UserEdge)> edgeOperator) {
    return [edgeOperator] (om::segment::UserEdge& userEdge) {
      auto returnEdge = edgeOperator(userEdge);
      if ((returnEdge.parentID != userEdge.parentID
          || returnEdge.childID != userEdge.childID) 
          && returnEdge.parentID != 0 && returnEdge.childID != 0) {
        log_errors << "Setting the desired edge to the actual edge " <<
          " Desired: " << userEdge.parentID << ", " << userEdge.childID << ")" <<
          " Actual: " << returnEdge.parentID << ", " << returnEdge.childID << ")" <<
          std::endl;
        userEdge.parentID = returnEdge.parentID;
        userEdge.childID = returnEdge.childID;
      }
    };
  }

  /*
   * Modify input SegIDSet with only it's distinct roots.
   * Generate a list of edges to join all these seg ids and append to member variable
   * joinEdges_
   */
  void transformToRootsAndUpdateJoinEdges(om::common::SegIDSet& segIDs) {
    SegmentationDataWrapper sdw(segmentationID_);
    om::common::SegIDSet newRoots;
    std::transform(segIDs.begin(), segIDs.end(),
        std::inserter(newRoots, newRoots.begin()), 
        [sdw](om::common::SegID segID) {
          return sdw.Segments()->FindRootID(segID);
        });
    if (newRoots.size() > 1) {
      auto idIter = newRoots.begin();
      om::common::SegID firstRootID = *idIter++;
      std::transform(idIter, newRoots.end(), std::back_inserter(joinEdges_),
          [firstRootID] (om::common::SegID segID) {
            return om::segment::UserEdge{firstRootID, segID, 1, true};
          });
    }
    std::swap(segIDs, newRoots);
  }

 public:
  OmSegmentMultiSplitActionImpl() {}

  OmSegmentMultiSplitActionImpl(const SegmentationDataWrapper& sdw,
                           const std::vector<om::segment::UserEdge>& splitEdges,
                           const om::common::SegIDSet& sources,
                           const om::common::SegIDSet& sinks)
      : splitEdges_(splitEdges), sources_(sources), sinks_(sinks),
        segmentationID_(sdw.GetSegmentationID()) {}

  void Execute() {
    SegmentationDataWrapper sdw(segmentationID_);

    // split all the input edges
    std::for_each(splitEdges_.begin(), splitEdges_.end(), updateEdgeWithAction(
          [sdw](om::segment::UserEdge userEdge) {
            return sdw.Segments()->SplitEdge(userEdge);
          }));

    // update the source/sink list to join only those who are now different segments
    joinEdges_.clear();
    transformToRootsAndUpdateJoinEdges(sources_);
    transformToRootsAndUpdateJoinEdges(sinks_);

    // perform the join
    std::for_each(joinEdges_.begin(), joinEdges_.end(), updateEdgeWithAction(
          [sdw](om::segment::UserEdge userEdge) {
            return sdw.Segments()->JoinEdge(userEdge).second;
          }));

    sdw.SegmentLists()->RefreshGUIlists();
  }

  void Undo() {
    SegmentationDataWrapper sdw(segmentationID_);

    // Split all the joined edges first
    std::for_each(joinEdges_.begin(), joinEdges_.end(), updateEdgeWithAction(
          [sdw](om::segment::UserEdge userEdge) {
            return sdw.Segments()->SplitEdge(userEdge);
          }));

    // Join all the split edges
    std::for_each(splitEdges_.begin(), splitEdges_.end(), updateEdgeWithAction(
          [sdw](om::segment::UserEdge userEdge) {
            return sdw.Segments()->JoinEdge(userEdge).second;
          }));
    sdw.SegmentLists()->RefreshGUIlists();
  }

  std::string Description() const {
    SegmentationDataWrapper sdw(segmentationID_);
    QString description = QString("MultiSplit Roots: %1, %2")
      .arg(sdw.Segments()->FindRootID(*sources_.begin()))
      .arg(sdw.Segments()->FindRootID(*sinks_.begin()));
    return description.toStdString();
  }


  QString classNameForLogFile() const { return "OmSegmentMultiSplitAction"; }

 private:
  template <typename T>
  friend class OmActionLoggerThread;
  friend class QDataStream& operator<<(QDataStream&,
                                       const OmSegmentMultiSplitActionImpl&);
  friend class QDataStream& operator>>(QDataStream&, OmSegmentMultiSplitActionImpl&);
  friend QTextStream& operator<<(QTextStream& out,
                                 const OmSegmentMultiSplitActionImpl&);
};
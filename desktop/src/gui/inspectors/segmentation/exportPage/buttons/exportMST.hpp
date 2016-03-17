#pragma once
#include "precomp.h"

#include "gui/inspectors/segmentation/exportPage/pageExport.h"
#include "gui/widgets/omButton.hpp"
#include "segment/omSegments.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"
#include "gui/widgets/segmentationDataWrapperButton.hpp"

namespace om {
namespace segmentationInspector {

class ExportMST : public SegmentationDataWrapperButton {
 public:
  ExportMST(QWidget* widget,
      const SegmentationDataWrapper& segmentationDataWrapper)
      : SegmentationDataWrapperButton(widget, "Export MST as text file",
          "export MST", false, segmentationDataWrapper) {}

 private:
  void onLeftClick() override {
    const QString outFile = OmProject::OmniFile() + ".mst.txt";

    QFile data(outFile);
    if (data.open(QFile::WriteOnly | QFile::Truncate)) {
      log_infos << "writing segment file " << qPrintable(outFile);
    } else {
      throw om::IoException("could not open file");
    }

    QTextStream out(&data);

    const SegmentationDataWrapper& sdw = GetSegmentationDataWrapper();
    OmSegments* segments = sdw.Segments();

    QStringList headerLabels;
    headerLabels << "Edge"
                 << "segID1"
                 << "segID2"
                 << "threshold"
                 << "userJoin"
                 << "userSplit"
                 << "seg1size"
                 << "seg2size";
    out << headerLabels.join(",");
    out << "\n";

    auto& edges = sdw.GetSegmentation()->MST();
    for (uint32_t i = 0; i < edges.size(); ++i) {
      OmSegment* node1 = segments->GetSegment(edges[i].node1ID);
      OmSegment* node2 = segments->GetSegment(edges[i].node2ID);

      out << i << ",";
      out << edges[i].node1ID << ",";
      out << edges[i].node2ID << ",";
      out << edges[i].threshold << ",";
      out << edges[i].userJoin << ",";
      out << edges[i].userSplit << ",";
      out << node1->size() << ",";
      out << node2->size();
      out << "\n";
    }

    log_infos << "\tdone!";
  }
};

}  // namespace segmentationInspector
}  // namespace om

#pragma once
#include "precomp.h"

#include "datalayer/fs/omFile.hpp"
#include "gui/inspectors/segmentation/exportPage/pageExport.h"
#include "gui/widgets/omButton.hpp"
#include "segment/omSegments.h"
#include "utility/dataWrappers.h"

namespace om {
namespace segmentationInspector {

class ExportSegmentList : public OmButton<PageExport> {
 public:
  ExportSegmentList(PageExport* d)
      : OmButton<PageExport>(d, "Export Segment Info (Valid)",
                             "export segment info", false) {}

 private:
  void doAction() {
    const SegmentationDataWrapper& sdw = mParent->GetSDW();

    OmSegments* segments = sdw.Segments();

    const QString outFile = OmProject::OmniFile() + ".segments.txt";

    QFile file(outFile);
    om::file::old::openFileWO(file);

    log_infos << "writing segment file " << qPrintable(outFile);

    QTextStream out(&file);
    out << "segID, 1 == working, 2 == valid, 3 == uncertain\n";
    out << "example: 100,2,0\n";

    for (om::common::SegID i = 1; i <= segments->getMaxValue(); ++i) {
      OmSegment* seg = segments->GetSegment(i);
      if (!seg) {
        continue;
      }

      int category = 0;
      switch (seg->GetListType()) {
        case om::common::SegListType::WORKING:
          category = 1;
          break;
        case om::common::SegListType::VALID:
          category = 2;
          break;
        case om::common::SegListType::UNCERTAIN:
          category = 3;
          break;
        default:
          throw om::ArgException("unknown type");
      }

      out << i << "," << category << "\n";
    }

    log_infos << "\tdone!";
  }
};

}  // namespace segmentationInspector
}  // namespace om

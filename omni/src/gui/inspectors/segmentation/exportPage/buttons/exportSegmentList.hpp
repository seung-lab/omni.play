#ifndef EXPORT_SEGMENT_LIST_HPP
#define EXPORT_SEGMENT_LIST_HPP

#include "gui/inspectors/segmentation/exportPage/pageExport.h"
#include "gui/widgets/omButton.hpp"
#include "segment/omSegments.h"
#include "system/omGroup.h"
#include "system/omGroups.h"
#include "utility/dataWrappers.h"

#include <QTextStream>

namespace om {
namespace segmentationInspector {

class ExportSegmentList : public OmButton<PageExport> {
public:
    ExportSegmentList(PageExport * d)
        : OmButton<PageExport>( d,
                                           "Export Segment Info (Valid)",
                                           "export segment info",
                                           false)
        , groups_(NULL)
    {}

private:
    OmGroups* groups_;

    void doAction()
    {
        const SegmentationDataWrapper& sdw = mParent->GetSDW();

        groups_ = sdw.GetSegmentation().Groups();

        OmSegments* segments = sdw.Segments();

        const QString outFile = OmProject::OmniFile() + ".segments.txt";

        QFile data(outFile);
        if(data.open(QFile::WriteOnly | QFile::Truncate)) {
            printf("writing segment file %s\n", qPrintable(outFile));
        } else{
            throw OmIoException("could not open file", outFile);
        }

        QTextStream out(&data);
        out << "segID, 1 == working, 2 == valid, 3 == uncertain, isGlia\n";
        out << "example: 100,2,0\n";

        for(OmSegID i = 1; i <= segments->getMaxValue(); ++i)
        {
            OmSegment* seg = segments->GetSegment(i);
            if(!seg){
                continue;
            }

            int category = 0;
            switch(seg->GetListType()){
            case om::WORKING:
                category = 1;
                break;
            case om::VALID:
                category = 2;
                break;
            case om::UNCERTAIN:
                category = 3;
                break;
            default:
                throw OmArgException("unknown type");
            }

            const int glia = isGlia(seg);

            out << i << "," << category << "," << glia << "\n";
        }

        printf("\tdone!\n");
    }

    int isGlia(OmSegment* seg)
    {
        const OmGroupIDsSet set = groups_->GetGroups(seg->RootID());

        if(set.empty()){
            return 0;
        }

        FOR_EACH(iter, set)
        {
            OmGroup& group = groups_->GetGroup(*iter);

            if("glia" == group.GetName()){
                return 1;
            }
        }

        return 0;
    }
};

} // namespace segmentationInspector
} // namespace om

#endif

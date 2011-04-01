#ifndef OM_SEGMENT_UTILS_HPP
#define OM_SEGMENT_UTILS_HPP

#include "utility/dataWrappers.h"
#include "segment/io/omValidGroupNum.hpp"
#include "segment/lists/omSegmentLists.h"
#include "segment/omSegment.h"
#include "segment/omSegments.h"
#include "segment/omSegmentIterator.h"
#include "segment/omSegmentSearched.hpp"
#include "segment/omSegmentSelected.hpp"

#include <QPixmap>

class OmSegmentUtils {
public:
    boost::shared_ptr<std::set<OmSegment*> >
    static GetAllChildrenSegments(OmSegments* segments,
                                  const OmSegIDsSet& set)
    {
        OmSegmentIterator iter(segments);
        iter.iterOverSegmentIDs(set);

        OmSegment * seg = iter.getNextSegment();
        std::set<OmSegment*>* children = new std::set<OmSegment*>();

        while(NULL != seg) {
            children->insert(seg);
            seg = iter.getNextSegment();
        }

        return boost::shared_ptr<std::set<OmSegment*> >(children);
    }

    static OmSegment* GetSegmentBasedOnThreshold(OmSegment* seg, const float breakThreshold)
    {
        // WARNING: operation is O(depth of MST)

        OmSegment* segWalker = seg;

        OmSegment* parent = segWalker->getParent();
        float segThreshold = segWalker->getThreshold();

        while(parent &&
              segThreshold > breakThreshold &&
              segThreshold < 2)
        {
            segWalker = parent;
            segThreshold = segWalker->getThreshold();
            parent = segWalker->getParent();
        }

        return segWalker;
    }

    inline static bool UseParentColorBasedOnThreshold(OmSegment* seg,
                                                      const float breakThreshold)
    {
        return seg->getParent() &&
            seg->getThreshold() > breakThreshold &&
            seg->getThreshold() < 2;
        // 2 is the manual merge threshold
    }


    static void PrintChildren(const SegmentDataWrapper& sdw)
    {
        if(!sdw.IsSegmentValid()){
            return;
        }

        OmSegments* segments = sdw.Segments();
        OmSegmentIterator iter(segments);
        iter.iterOverSegmentID(sdw.FindRootID());

        OmSegment * seg = iter.getNextSegment();
        while(NULL != seg) {
            OmSegment* parent = seg->getParent();
            OmSegID parentID = 0;
            if(parent){
                parentID = parent->value();
            }
            const QString str = QString("%1 : %2, %3, %4")
                .arg(seg->value())
                .arg(parentID)
                .arg(seg->getThreshold())
                .arg(seg->size());
            printf("%s\n", qPrintable(str));
            seg = iter.getNextSegment();
        }
    }

    static void ReValidateEveryObject(const SegmentationDataWrapper& sdw)
    {
        OmSegments* segments = sdw.Segments();

        OmValidGroupNum* validGroupNum = sdw.ValidGroupNum();

        for(uint32_t i = 1; i <= segments->getMaxValue(); ++i)
        {
            OmSegment* seg = segments->GetSegment(i);

            // only process valid, root (i.e. parent == 0) segments
            if(!seg || !seg->IsValidListType() || seg->getParent()){
                continue;
            }

            OmSegIDsSet set;
            set.insert(i);

            boost::shared_ptr<std::set<OmSegment*> > children =
                OmSegmentUtils::GetAllChildrenSegments(segments, set);

            validGroupNum->Set(children, true);
        }
    }

    static OmSegID GetNextSegIDinWorkingList(const SegmentDataWrapper& sdw){
        return sdw.GetSegmentation().SegmentLists()->GetNextSegIDinWorkingList(sdw);
    }

    static OmSegID GetNextSegIDinWorkingList(const SegmentationDataWrapper& sdw){
        return sdw.SegmentLists()->GetNextSegIDinWorkingList(sdw);
    }

    static std::string ListTypeAsStr(const om::SegListType type)
    {
        switch(type){
        case om::WORKING:
            return "working";
        case om::VALID:
            return "valid";
        case om::UNCERTAIN:
            return "uncertain";
        default:
            return "unknown type";
        }
    }

    static QColor SetSegColor(const SegmentDataWrapper& sdw, const QColor& color)
    {
        // max allowed color val is 128 to allow color to be highlighted

        const OmColor c = { std::min(128, color.red()),
                            std::min(128, color.green()),
                            std::min(128, color.blue()) };
        sdw.SetColor(c);

        return OmColorToQColor(c);
    }

    static QColor SegColorAsQColor(const SegmentDataWrapper& sdw){
        return OmColorToQColor(sdw.GetColorInt());
    }

    static QColor OmColorToQColor(const OmColor color){
        return qRgb(color.red, color.green, color.blue);
    }

    static QPixmap SegColorAsQPixmap(const SegmentDataWrapper& sdw,
                                     const int width = 40,
                                     const int height = 30)
    {
        const QColor newcolor = SegColorAsQColor(sdw);
        QPixmap pixm(width, height);
        pixm.fill(newcolor);
        return pixm;
    }
};
#endif

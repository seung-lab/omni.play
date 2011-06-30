#pragma once

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
    om::shared_ptr<std::set<OmSegment*> >
    static GetAllChildrenSegments(OmSegments* segments, const OmSegIDsSet& set)
    {
        OmSegmentIterator iter(segments);
        iter.iterOverSegmentIDs(set);

        OmSegment* seg = iter.getNextSegment();

        std::set<OmSegment*>* children = new std::set<OmSegment*>();

        while(NULL != seg)
        {
            children->insert(seg);
            seg = iter.getNextSegment();
        }

        return om::shared_ptr<std::set<OmSegment*> >(children);
    }

    template <class A, class B>
    static void GetAllChildrenSegments(const SegmentationDataWrapper& sdw, const A& segsIn, B& ret)
    {
        OmSegmentIterator iter(sdw);
        iter.iterOverSegmentIDs(segsIn);

        OmSegment* seg = iter.getNextSegment();

        while(NULL != seg)
        {
            ret.push_back(seg);
            seg = iter.getNextSegment();
        }
    }

    template <class B>
    static void GetAllChildrenSegments(OmSegments* segments, const OmSegID segID, B& ret)
    {
        OmSegmentIterator iter(segments);
        iter.iterOverSegmentID(segID);

        OmSegment* seg = iter.getNextSegment();

        while(NULL != seg)
        {
            ret.push_back(seg);
            seg = iter.getNextSegment();
        }
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

    om::shared_ptr<std::deque<std::string> >
    static GetChildrenInfo(const SegmentDataWrapper& sdw)
    {
        om::shared_ptr<std::deque<std::string> > ret =
            om::make_shared<std::deque<std::string> >();

        if(!sdw.IsSegmentValid()){
            return ret;
        }

        OmSegmentIterator iter(sdw);
        iter.iterOverSegmentID(sdw.FindRootID());

        OmSegment* seg = iter.getNextSegment();

        while(NULL != seg)
        {
            OmSegment* parent = seg->getParent();

            const OmSegID parentID = parent ? parent->value() : 0;

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

            std::deque<OmSegment*> children;
            OmSegmentUtils::GetAllChildrenSegments(segments, i, children);

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
        // const OmColor c = { std::min(128, color.red()),
        //                     std::min(128, color.green()),
        //                     std::min(128, color.blue()) };

        const OmColor c = { color.red(),
                            color.green(),
                            color.blue() };

        sdw.SetColor(c);

        return OmColorToQColor(c);
    }

    static QColor SegColorAsQColor(const SegmentDataWrapper& sdw){
        return OmColorToQColor(sdw.GetColorInt());
    }

    static QColor OmColorToQColor(const OmColor color){
        return qRgb(color.red, color.green, color.blue);
    }

    static QPixmap SegColorAsQPixmap(const SegmentDataWrapper& sdw)
    {
        const QColor newcolor = SegColorAsQColor(sdw);
        return MakePixMap(newcolor);
    }

    static QPixmap MakePixMap(const QColor newcolor,
                              const int width = 40,
                              const int height = 30)
    {
        QPixmap pixm(width, height);
        pixm.fill(newcolor);
        return pixm;
    }

    template <class A, class B>
    static void GetSegPtrs(const SegmentationDataWrapper& sdw, const A& ids, B& segPtrs)
    {
        OmSegments* cache = sdw.Segments();

        FOR_EACH(iter, ids)
        {
            OmSegment* seg = cache->GetSegment(*iter);

            if(!seg){
                continue;
            }

            segPtrs.push_back(seg);
        }
    }
};

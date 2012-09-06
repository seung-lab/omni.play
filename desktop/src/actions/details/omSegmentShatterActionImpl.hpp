#pragma once

#include "common/omCommon.h"
#include "events/omEvents.h"
#include "project/omProject.h"
#include "segment/lists/omSegmentLists.h"
#include "system/cache/omCacheManager.h"
#include "utility/dataWrappers.h"
#include "viewGroup/omViewGroupState.h"

class OmSegmentShatterActionImpl {
private:
    SegmentDataWrapper sdw_;
    QString desc;

public:
    OmSegmentShatterActionImpl()
    {}

    OmSegmentShatterActionImpl(const SegmentDataWrapper& sdw)
        : sdw_(sdw)
        , desc("Shattering: ")
    {}

    void Execute()
    {
    	sdw_.GetSegmentation().Segments()->Shatter(sdw_.GetSegment());

        desc = QString("Shatter seg %1").arg(sdw_.GetSegmentID());
        notify();
    }

    void Undo()
    {


        desc = QString("Undo shatter currently not supported").arg(sdw_.GetSegmentID());
		notify();
    }

    std::string Description() const {
        return desc.toStdString();
    }

    QString classNameForLogFile() const {
        return "OmSegmentShatterAction";
    }

private:
	void notify() const
	{
		std::cout << desc.toStdString() << "\n";

        OmEvents::SegmentModified();

        sdw_.GetSegmentation().SegmentLists()->RefreshGUIlists();

        OmCacheManager::TouchFreshness();
        OmEvents::Redraw2d();
        OmEvents::Redraw3d();
	}

    template <typename T> friend class OmActionLoggerThread;
    friend class QDataStream &operator<<(QDataStream&, const OmSegmentShatterActionImpl&);
    friend class QDataStream &operator>>(QDataStream&, OmSegmentShatterActionImpl&);
    friend QTextStream& operator<<(QTextStream& out, const OmSegmentShatterActionImpl&);
};


#pragma once

#include "landmarks/omLandmarksTypes.h"
#include "landmarks/omLandmarksDialog.h"

// no locking needed--assume always called from GUI thread

class OmLandmarks {
private:
    QWidget* parent_;
    boost::scoped_ptr<om::landmarks::dialog> dialog_;

    std::set<SegmentDataWrapper> segments_;
    std::vector<om::landmarks::sdwAndPt> pts_;

public:
    OmLandmarks(QWidget* parent)
        : parent_(parent)
        , dialog_(NULL)
    {}

    void Add(boost::optional<SegmentDataWrapper> sdwIn,
             const om::globalCoord& dataClickPoint)
    {
        if(!sdwIn){
            return;
        }

        const SegmentDataWrapper sdw = *sdwIn;

        if(!sdw.IsValidWrapper()){
            return;
        }

        if(segments_.count(sdw))
        {
            std::cout << "skipping " << sdw << ": already present\n";
            return;
        }

        om::landmarks::sdwAndPt s = { sdw, dataClickPoint };

        pts_.push_back(s);
        segments_.insert(sdw);

        if(2 <= pts_.size()){
            showMenu();
        }
    }

    void ClearPts()
    {
        pts_.clear();
        segments_.clear();
    }

private:
    void showMenu()
    {
        if(dialog_ == NULL) {
            dialog_.reset(new om::landmarks::dialog(parent_, this));   
        }
        dialog_->Reset(pts_);
        dialog_->show();
    }
};

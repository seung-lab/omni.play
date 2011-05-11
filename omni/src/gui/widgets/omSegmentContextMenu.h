#pragma once

#include "common/omCommon.h"
#include "utility/dataWrappers.h"

#include <QMenu>

class OmViewGroupState;
class OmSegmentPickPoint;
namespace om { namespace gui { class progressBarDialog; } }

class OmSegmentContextMenu : public QMenu {

Q_OBJECT

public:
    void Refresh(const SegmentDataWrapper& sdw, OmViewGroupState* vgs);
    void Refresh(const SegmentDataWrapper& sdw, OmViewGroupState* vgs, const DataCoord coord);
    void Refresh(const OmSegmentPickPoint& pickPoint, OmViewGroupState* vgs);

protected:
    void addSelectionNames();
    void addSelectionAction();
    void addDendActions();
    void addColorActions();
    void addGroupActions();
    void addPropertiesActions();
    void addGroups();
    void addDisableAction();

private Q_SLOTS:
    void select();
    void unselect();
    void unselectOthers();

    void mergeSegments();
    void splitSegments();
    void cutSegments();
    void randomizeColor();
    void randomizeSegmentColor();
    void setSelectedColor();

    void setValid();
    void setNotValid();
    void showProperties();
    void printChildren();

    void disableSegment();

private:
    SegmentDataWrapper sdw_;
    DataCoord coord_;
    OmViewGroupState* vgs_;

    bool isValid() const;
    bool isUncertain() const;

    void doRefresh();

    static void writeChildrenFile(const QString fnp, om::gui::progressBarDialog* dialog,
                                  om::shared_ptr<std::deque<std::string> > children);
};

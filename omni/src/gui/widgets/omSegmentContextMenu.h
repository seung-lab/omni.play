#ifndef OM_SEGMENT_CONTEXT_MENU_H
#define OM_SEGMENT_CONTEXT_MENU_H

#include "common/omCommon.h"
#include "utility/dataWrappers.h"

#include <QMenu>

class OmViewGroupState;
class OmSegmentPickPoint;

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

    bool isValid() const;
    bool isUncertain() const;

    void doRefresh();

    OmViewGroupState* vgs_;
};

#endif

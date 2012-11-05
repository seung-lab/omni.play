#pragma once

/* Widget for editing and inspecting groups.
 *
* Author(s): Matt Wimer
 */

#include "common/common.h"
#include "project/omProject.h"
#include "system/omGroups.h"
#include "gui/widgets/omGroupListWidget.h"
#include "utility/segmentationDataWrapper.hpp"

#include <QTableWidget>
#include <QGridLayout>
#include <QTreeWidget>
#include <QTableView>
#include <QMenu>

class OmViewGroupState;

class GroupsTable : public QWidget {
Q_OBJECT
public:
    GroupsTable(OmViewGroupState* vgs);

    static void Repopulate(om::common::SegID id = 0);

    void populateGroupTable(om::common::GroupID id);

    void SetSegmentID(const om::common::SegID seg){
        seg_ = seg;
    }

private Q_SLOTS:
    void doDeleteAction();

private:
    OmViewGroupState *const vgs_;
    const SegmentationDataWrapper sdw_;

    om::common::SegID seg_;
    om::common::GroupID groupid_;

    void populateGroupsList();

    QGridLayout* mLayout;
    OmGroupListWidget* mGroupsList;
    QTableWidget* mGroupsTable;
    QMenu* mMenu;
};

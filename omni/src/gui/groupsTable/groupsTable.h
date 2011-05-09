#ifndef __GROUPS_TABLE_H__
#define __GROUPS_TABLE_H__

/* Widget for editing and inspecting groups.
 *
* Author(s): Matt Wimer
 */

#include "common/omCommon.h"
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

    static void Repopulate(OmSegID id = 0);

    void populateGroupTable(OmGroupID id);

    void SetSegmentID(const OmSegID seg){
        seg_ = seg;
    }

private Q_SLOTS:
    void doDeleteAction();

private:
    OmViewGroupState *const vgs_;
    const SegmentationDataWrapper sdw_;

    OmSegID seg_;
    OmGroupID groupid_;

    void populateGroupsList();

    QGridLayout* mLayout;
    OmGroupListWidget* mGroupsList;
    QTableWidget* mGroupsTable;
    QMenu* mMenu;
};
#endif


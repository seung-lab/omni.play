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

#include <QTableWidget>
#include <QGridLayout>
#include <QTreeWidget>
#include <QTableView>

class OmViewGroupState;

class GroupsTable : public QWidget
{
        Q_OBJECT
public:
        GroupsTable(OmViewGroupState * vgs);

	void populateGroupTable(OmGroupID id);

private:
        OmId getSegmentationID();

        void populateGroupsList();

	OmViewGroupState * mViewGroupState;

        QGridLayout * mLayout;
	OmGroupListWidget * mGroupsList;
	QTableWidget * mGroupsTable;
};
#endif


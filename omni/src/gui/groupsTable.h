#ifndef __GROUPS_TABLE_H__
#define __GROUPS_TABLE_H__
/* Widget for editing and inspecting groups.
 * 
 * Author(s): Matt Wimer
 */
#include "common/omCommon.h"
#include "project/omProject.h"
#include "system/omGroups.h"

#include <QTableView>
#include <QGridLayout>
#include <QTreeWidget>
#include <QTableView>

class OmViewGroupState;

class GroupsTable : public QWidget
{
        Q_OBJECT
public:
        GroupsTable(OmViewGroupState * vgs);

        OmId getSegmentationID();
        void populateGroupsList();

private:
	OmViewGroupState * mViewGroupState;

        QGridLayout * mLayout;
	QTreeWidget * mGroupsList;
	QTableView * mGroupsTable;
};
#endif


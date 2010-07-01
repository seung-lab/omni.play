#include "gui/groupsTable.h"
#include "system/viewGroup/omViewGroupState.h"
#include "volume/omSegmentation.h"
#include "common/omCommon.h"
#include "system/omGroup.h"
#include <QTreeWidgetItem>

static OmId mSeg = 1; //FIXME!

GroupsTable::GroupsTable(OmViewGroupState * vgs) : QWidget(), mViewGroupState(vgs)
{
	mLayout = new QGridLayout();
	this->setLayout(mLayout);

	mGroupsList = new QTreeWidget();
	mLayout->addWidget(mGroupsList,0,0,1,1);

	mGroupsTable = new QTableView();
	mLayout->addWidget(mGroupsTable,0,1,1,1);

	populateGroupsList();
}

OmId GroupsTable::getSegmentationID()
{
        return mSeg;
}

void GroupsTable::populateGroupsList()
{
	OmSegmentation & seg = OmProject::GetSegmentation(getSegmentationID());
	OmGroups * groups = seg.GetGroups();

	OmGroupIDsSet set = groups->GetGroups();

	foreach(OmGroupID id, set) {
                QTreeWidgetItem *row = new QTreeWidgetItem(mGroupsList);
                OmGroup & group = groups->GetGroup(id);
                row->setText(0, group.GetName());
                row->setData(0, Qt::UserRole, qVariantFromValue(id));
        	row->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

		printf("here\n");
	}
}

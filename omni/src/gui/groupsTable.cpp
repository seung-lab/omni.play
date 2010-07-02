#include "gui/groupsTable.h"
#include "system/viewGroup/omViewGroupState.h"
#include "volume/omSegmentation.h"
#include "segment/omSegmentCache.h"
#include "common/omCommon.h"
#include "system/omGroup.h"
#include <QTreeWidgetItem>

static OmId mSeg = 1; //FIXME!

GroupsTable::GroupsTable(OmViewGroupState * vgs) : QWidget(), mViewGroupState(vgs)
{
	mLayout = new QGridLayout();
	this->setLayout(mLayout);

	mGroupsList = new OmGroupListWidget(this);
	mLayout->addWidget(mGroupsList,0,0,1,1);

	mGroupsTable = new QTableWidget();
	mGroupsTable->setRowCount(10);
	mGroupsTable->setColumnCount(2);
	mLayout->addWidget(mGroupsTable,0,1,1,1);

	populateGroupsList();
}

OmId GroupsTable::getSegmentationID()
{
        return mSeg;
}

void GroupsTable::populateGroupsList()
{
	if(!OmProject::IsSegmentationValid(getSegmentationID())) {
		return;
	}
	mGroupsList->populate(OmProject::GetSegmentation(getSegmentationID()));
}

void GroupsTable::populateGroupTable(OmGroupID id)
{
	if(!OmProject::IsSegmentationValid(getSegmentationID())) {
		return;
	}
        OmSegmentation & seg = OmProject::GetSegmentation(getSegmentationID());
        OmGroup & group = seg.GetGroups()->GetGroup(id);
	OmSegmentCache * cache = seg.GetSegmentCache();

	const OmSegIDsSet & set = group.GetIDs();
	int count = 0;
	foreach(OmSegID id, set) {
		OmSegment * segment = cache->GetSegment(id);
		OmColor color = segment->GetColorInt();
		printf("HI! %i\n", id);
		QPushButton * segmentButton = new QPushButton(QString("%1").arg(id));
		mGroupsTable->setCellWidget(count, 0, segmentButton);
		QPushButton * colorButton = new QPushButton();

		colorButton->setStyleSheet(QString("* { background-color: rgb(%1,%2,%3) }").arg(color.red).arg(color.green).arg(color.blue));
		mGroupsTable->setCellWidget(count, 1, colorButton);
		count++;
	}
}


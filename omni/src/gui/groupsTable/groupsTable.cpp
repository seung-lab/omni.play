#include "gui/groupsTable/groupsTable.h"
#include "viewGroup/omViewGroupState.h"
#include "volume/omSegmentation.h"
#include "segment/omSegmentCache.h"
#include "common/omCommon.h"
#include "system/omGroup.h"
#include "gui/groupsTable/dropdownMenuButton.hpp"

#include <QTreeWidgetItem>
#include <QMenu>

static OmID mSeg = 1; //FIXME!
static GroupsTable * gTable = NULL;

void GroupsTable::SetSegmentID(OmSegID seg)
{
	seg_ = seg;
}

GroupsTable::GroupsTable(OmViewGroupState * vgs) : QWidget(), mViewGroupState(vgs)
{
	mLayout = new QGridLayout();
	this->setLayout(mLayout);

	mGroupsList = new OmGroupListWidget(this);
	mGroupsList->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
	mLayout->addWidget(mGroupsList,0,0,1,1);

	mGroupsTable = new QTableWidget();
	mGroupsTable->setColumnCount(4);
	mGroupsTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	mLayout->addWidget(mGroupsTable,0,1,1,1);

	mMenu = new QMenu ();
	mMenu->addSeparator();
	QAction * delAction = new QAction(QString("Remove"), this);
	connect(delAction, SIGNAL(triggered(bool)),
			this, SLOT(doDeleteAction()));

	mMenu->addAction(delAction);

	populateGroupsList();
	gTable = this;
}

void GroupsTable::doDeleteAction()
{
	printf("here %p %p %u %u\n", QObject::sender(), mMenu, seg_, groupid_);

	if(!OmProject::IsSegmentationValid(GetSegmentationID())) {
		return;
	}
	OmSegmentation & seg = OmProject::GetSegmentation(GetSegmentationID());
	OmGroup & group = seg.GetGroups()->GetGroup(groupid_);

	OmSegIDsSet set;
	set.insert(seg_);
	seg.GetGroups()->UnsetGroup(set, group.GetName());
}

OmID GroupsTable::GetSegmentationID()
{
	return mSeg;
}

void GroupsTable::populateGroupsList()
{
	if(!OmProject::IsSegmentationValid(GetSegmentationID())) {
		return;
	}
	printf("repopulating..\n");
	mGroupsList->populate(OmProject::GetSegmentation(GetSegmentationID()));
}

void GroupsTable::Repopulate(OmSegID id)
{
	if(!gTable) {
		return;
	}
	gTable->populateGroupsList();

	if(id) {
		gTable->populateGroupTable(id);
	}
}

void GroupsTable::populateGroupTable(OmGroupID id)
{
	groupid_ = id;

	if(!OmProject::IsSegmentationValid(GetSegmentationID())) {
		return;
	}

	OmSegmentation & seg = OmProject::GetSegmentation(GetSegmentationID());
	OmGroup & group = seg.GetGroups()->GetGroup(id);
	OmSegmentCache* cache = seg.GetSegmentCache();

	const OmSegIDsSet & set = group.GetIDs();
	mGroupsTable->setRowCount(set.size());
	int count = 0;

	foreach(OmSegID id, set) {
		OmSegment * segment = cache->GetSegment(id);
		OmColor color = segment->GetColorInt();

		GroupsTableDropDownMenuButton* segmentButton =
			new GroupsTableDropDownMenuButton(this, id);
		segmentButton->setMenu(mMenu);

		mGroupsTable->setCellWidget(count, 0, segmentButton);

		QPushButton * colorButton = new QPushButton();
		colorButton->setStyleSheet(QString("* { background-color: rgb(%1,%2,%3) }")
								   .arg(color.red).arg(color.green).arg(color.blue));
		mGroupsTable->setCellWidget(count, 1, colorButton);
		mGroupsTable->setItem(count, 2, new QTableWidgetItem(QString::number(segment->getSizeWithChildren()), 0));
		mGroupsTable->setItem(count, 3, new QTableWidgetItem(QString::number(segment->size()), 0));
		count++;
	}
}


#include "gui/groupsTable.h"
#include "viewGroup/omViewGroupState.h"
#include "volume/omSegmentation.h"
#include "segment/omSegmentCache.h"
#include "common/omCommon.h"
#include "system/omGroup.h"
#include <QTreeWidgetItem>
#include <QMenu>

static OmID mSeg = 1; //FIXME!
static GroupsTable * gTable = NULL;

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
/*
        connect(delAction, SIGNAL(triggered(bool)),
                this, SLOT(doDeleteAction()));
*/

	mMenu->addAction(delAction);

	populateGroupsList();
	gTable = this;
}

void GroupsTable::doDeleteAction()
{
	printf("here %p %p\n", QObject::sender(), mMenu);
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
	mGroupsList->populate(OmProject::GetSegmentation(GetSegmentationID()));
}

void GroupsTable::Repopulate()
{
	if(!gTable) {
		return;
	}
	gTable->populateGroupsList();
}

void GroupsTable::populateGroupTable(OmGroupID id)
{
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
		//printf("HI! %i\n", id);
		QPushButton * segmentButton = new QPushButton(QString("%1").arg(id));
		segmentButton->setMenu(mMenu);
	        connect(segmentButton, SIGNAL(clicked(bool)),
                	this, SLOT(doDeleteAction()));


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


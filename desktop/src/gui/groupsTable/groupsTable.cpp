#include "segment/omSegment.h"
#include "system/omConnect.hpp"
#include "common/common.h"
#include "gui/groupsTable/dropdownMenuButton.hpp"
#include "gui/groupsTable/groupsTable.h"
#include "segment/omSegments.h"
#include "system/omGroup.h"
#include "viewGroup/omViewGroupState.h"
#include "volume/omSegmentation.h"

#include <QTreeWidgetItem>
#include <QMenu>

// TODO: fixme!
static GroupsTable* gTable = nullptr;

GroupsTable::GroupsTable(OmViewGroupState& vgs)
    : QWidget(), vgs_(vgs), sdw_(vgs.Segmentation()) {
  mLayout = new QGridLayout();
  this->setLayout(mLayout);

  mGroupsList = new OmGroupListWidget(this);
  mGroupsList->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
  mLayout->addWidget(mGroupsList, 0, 0, 1, 1);

  mGroupsTable = new QTableWidget();
  mGroupsTable->setColumnCount(4);
  mGroupsTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  mLayout->addWidget(mGroupsTable, 0, 1, 1, 1);

  mMenu = new QMenu();
  mMenu->addSeparator();
  QAction* delAction = new QAction(QString("Remove"), this);
  om::connect(delAction, SIGNAL(triggered(bool)), this, SLOT(doDeleteAction()));

  mMenu->addAction(delAction);

  populateGroupsList();
  gTable = this;
}

void GroupsTable::doDeleteAction() {
  if (!sdw_.IsSegmentationValid()) {
    return;
  }

  OmGroup& group = sdw_.Groups()->GetGroup(groupid_);

  om::common::SegIDSet set;
  set.insert(seg_);
  sdw_.Groups()->UnsetGroup(set, group.GetName());
}

void GroupsTable::populateGroupsList() {
  if (!sdw_.IsSegmentationValid()) {
    return;
  }

  log_debugs(unknown) << "repopulating..";
  mGroupsList->populate(sdw_.GetSegmentation());
}

void GroupsTable::Repopulate(om::common::SegID id) {
  if (!gTable) {
    return;
  }

  gTable->populateGroupsList();

  if (id) {
    gTable->populateGroupTable(id);
  }
}

void GroupsTable::populateGroupTable(om::common::GroupID id) {
  groupid_ = id;

  if (!sdw_.IsSegmentationValid()) {
    return;
  }

  OmGroup& group = sdw_.Groups()->GetGroup(id);
  OmSegments* cache = sdw_.Segments();

  const om::common::SegIDSet& set = group.ids();
  mGroupsTable->setRowCount(set.size());
  int count = 0;

  for (auto id : set) {
    ++count;

    OmSegment* segment = cache->GetSegment(id);
    const om::common::Color color = segment->GetColorInt();

    GroupsTableDropDownMenuButton* segmentButton =
        new GroupsTableDropDownMenuButton(this, id);
    segmentButton->setMenu(mMenu);

    mGroupsTable->setCellWidget(count, 0, segmentButton);

    QPushButton* colorButton = new QPushButton();
    colorButton->setStyleSheet(QString("* { background-color: rgb(%1,%2,%3) }")
                                   .arg(color.red).arg(color.green)
                                   .arg(color.blue));
    mGroupsTable->setCellWidget(count, 1, colorButton);
    mGroupsTable->setItem(count, 2,
                          new QTableWidgetItem(QString::number(0), 0));
    mGroupsTable->setItem(
        count, 3, new QTableWidgetItem(QString::number(segment->size()), 0));
  }
}

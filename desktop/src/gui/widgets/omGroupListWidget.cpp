#include "common/common.h"
#include "gui/guiUtils.hpp"
#include "gui/inspectors/inspectorProperties.h"
#include "gui/widgets/omGroupListWidget.h"
#include "system/omGroup.h"
#include "gui/groupsTable/groupsTable.h"
#include "viewGroup/omViewGroupState.h"

OmGroupListWidget::OmGroupListWidget(GroupsTable* gt)
    : QTreeWidget(), mGroupsTable(gt) {
  setSelectionMode(QAbstractItemView::ExtendedSelection);
  setAlternatingRowColors(true);
  setColumnCount(3);

  QStringList headers;
  headers << tr("Name") << tr("ID");
  setHeaderLabels(headers);

  setFocusPolicy(Qt::StrongFocus);
}

std::string OmGroupListWidget::eventSenderName() { return "groupList"; }

void OmGroupListWidget::populateTable() {
  QTreeWidgetItem* current = currentItem();
  if (!current) {
    return;
  }
  QVariant result = current->data(0, Qt::UserRole);
  printf("group: %i\n", result.toUInt());
  mGroupsTable->populateGroupTable(result.toUInt());
}

void OmGroupListWidget::mousePressEvent(QMouseEvent* event) {
  QTreeWidget::mousePressEvent(event);

  populateTable();

  if (event->button() == Qt::LeftButton) {
  } else if (event->button() == Qt::RightButton) {
  }
}

void OmGroupListWidget::keyPressEvent(QKeyEvent* event) {
  QTreeWidget::keyPressEvent(event);

  switch (event->key()) {
    case Qt::Key_Up:
    case Qt::Key_Down:
      populateTable();
      break;
  }
}

void OmGroupListWidget::populate(OmSegmentation& seg) {
  OmGroups* groups = seg.Groups();

  clear();

  auto set = groups->GetGroups();

  auto firstID = 0;
  for (const auto& id : set) {
    if (!firstID) {
      firstID = id;
    }
    auto* row = new QTreeWidgetItem(this);
    OmGroup& group = groups->GetGroup(id);
    row->setText(0, QString::fromStdString(group.GetName()));
    row->setData(0, Qt::UserRole, qVariantFromValue(id));
    row->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable |
                  Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

    printf("here\n");
  }
}

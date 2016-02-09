#include "common/common.h"
#include "gui/guiUtils.hpp"
#include "gui/inspectors/segmentInspector.h"
#include "gui/segmentLists/details/segmentListBase.h"
#include "gui/widgets/omSegmentContextMenu.h"
#include "gui/segmentLists/omSegmentListWidget.h"
#include "segment/lists/omSegmentListsTypes.hpp"
#include "segment/omSegmentSelector.h"
#include "segment/omSegmentUtils.hpp"
#include "segment/selection.hpp"
#include "events/events.h"
#include "utility/dataWrappers.h"
#include "utility/omStringHelpers.h"
#include "viewGroup/omViewGroupState.h"
#include "zi/omUtility.h"

Q_DECLARE_METATYPE(SegmentDataWrapper);

OmSegmentListWidget::OmSegmentListWidget(SegmentListBase* slist,
                                         OmViewGroupState& vgs)
    : QTreeWidget(slist), segmentListBase(slist), vgs_(vgs) {
  setSelectionMode(QAbstractItemView::ExtendedSelection);
  setAlternatingRowColors(true);

  QStringList headers;
  headers << tr("ID") << tr("Num Pieces") << tr("Total Size");
  setColumnCount(headers.size());
  setHeaderLabels(headers);
  // set the stylesheet for coloring selected segments, qpalette selected-background-color seems to be overwritten somewhere
  setStyleSheet("QTreeWidget::item:selected{background-color:rgb(150,177,220); show-decoration-selected:1;}");
  setFocusPolicy(Qt::StrongFocus);
}

bool OmSegmentListWidget::populate(const bool doScrollToSelectedSegment,
                                   const SegmentDataWrapper segmentJustSelected,
                                   std::shared_ptr<GUIPageOfSegments> segIDs) {
  bool makeTabActive = false;
  const om::common::SegID segIDjustSelected =
      segmentJustSelected.GetSegmentID();
  auto sdw = segmentJustSelected.MakeSegmentationDataWrapper();
  om::segment::Selection* selection = nullptr;
  if (sdw.IsValidWrapper()) {
    selection = &sdw.Segments()->Selection();
  }

  setUpdatesEnabled(false);
  clear();
  selectionModel()->blockSignals(true);
  selectionModel()->clearSelection();

  QTreeWidgetItem* rowToJumpTo = nullptr;

  assert(100 >= segIDs->segs.size() && "too many segments returned");

  for(auto& iter : segIDs->segs) {
    OmSegment* seg = iter.seg;
    if (!seg || !seg->value()) {
      log_errors << "Invalid segment in SegInfo for " << eventSenderName();
      continue;
    }

    QTreeWidgetItem* row = new QTreeWidgetItem(this);
    setRowFlags(row);

    row->setText(ID_COL, OmStringHelpers::HumanizeNumQT(seg->value(), '-'));

    SegmentDataWrapper segDW(sdw, seg->value());
    row->setData(USER_DATA_COL, Qt::UserRole, qVariantFromValue(segDW));

    const uint32_t numPieces = iter.numChildren + 1;
    row->setText(NUM_PIECES_COL, OmStringHelpers::HumanizeNumQT(numPieces));
    row->setTextAlignment(NUM_PIECES_COL, Qt::AlignHCenter);

    const int64_t sizeWithChildren = iter.sizeIncludingChildren;
    row->setText(SIZE_COL, OmStringHelpers::HumanizeNumQT(sizeWithChildren));
    row->setTextAlignment(SIZE_COL, Qt::AlignHCenter);

    if (selection) {
      row->setSelected(selection->IsSegmentSelected(seg));
    }

    if (doScrollToSelectedSegment) {
      if (seg->value() == segIDjustSelected) {
        rowToJumpTo = row;
        makeTabActive = true;
      }
    }
  }

  selectionModel()->blockSignals(false);

  GuiUtils::autoResizeColumnWidths(this, 3);

  if (doScrollToSelectedSegment && rowToJumpTo != nullptr) {
    scrollToItem(rowToJumpTo);
  }

  setUpdatesEnabled(true);

  return makeTabActive;
}

std::string OmSegmentListWidget::eventSenderName() { return "segmentList"; }

void OmSegmentListWidget::segmentLeftClick() {
  segmentListBase->userJustClickedInThisSegmentList();

  QTreeWidgetItem* current = currentItem();
  if (nullptr == current) {
    log_errors << "current segment not set";
    return;
  }
  QVariant result = current->data(USER_DATA_COL, Qt::UserRole);
  SegmentDataWrapper current_sdw = result.value<SegmentDataWrapper>();

  OmSegmentSelector sel(current_sdw.MakeSegmentationDataWrapper(), this,
                        eventSenderName());
  sel.AddToRecentList(
      segmentListBase->shouldSelectedSegmentsBeAddedToRecentList());

  bool isModifyingExistingSelection = QApplication::keyboardModifiers() & Qt::ControlModifier ||
      QApplication::keyboardModifiers() & Qt::ShiftModifier;

  if (!isModifyingExistingSelection) {
    sel.selectNoSegments();
  } else {
    Q_FOREACH(QTreeWidgetItem * item, selectedItems()) {
      QVariant result = item->data(USER_DATA_COL, Qt::UserRole);
      SegmentDataWrapper item_sdw = result.value<SegmentDataWrapper>();

      // the current row item (current_sdw) will be handled outside of this if/else below
      if (item_sdw.GetID() != current_sdw.GetID()) {
        sel.augmentSelectedSet(item_sdw.GetID(), item->isSelected());
      }
    }
  }

  /* 
   * Select or deselect the row that was clicked. Allows for deselect when we are modifying
   * the existing selection (ctrl-clicked), since we couldn't have modified it above in the
   * selecetedItems() for loop
   */
  sel.augmentSelectedSet(current_sdw.GetID(), current->isSelected());

  sel.AutoCenter(true);
}

void OmSegmentListWidget::mousePressEvent(QMouseEvent* event) {
  QTreeWidget::mousePressEvent(event);

  if (event->button() == Qt::LeftButton) {
    segmentLeftClick();
  } else if (event->button() == Qt::RightButton) {
    segmentRightClick(event);
  }
}

void OmSegmentListWidget::segmentRightClick(QMouseEvent* event) {
  if (!isSegmentSelected()) {
    return;
  }

  segmentShowContexMenu(event);
}

void OmSegmentListWidget::segmentShowContexMenu(QMouseEvent* event) {
  SegmentDataWrapper sdw = getCurrentlySelectedSegment();

  mSegmentContextMenu.Refresh(sdw, vgs_);
  mSegmentContextMenu.exec(event->globalPos());
}

bool OmSegmentListWidget::isSegmentSelected() {
  if (nullptr == currentItem()) {
    return false;
  }
  return true;
}

SegmentDataWrapper OmSegmentListWidget::getCurrentlySelectedSegment() {
  QTreeWidgetItem* segmentItem = currentItem();
  QVariant result = segmentItem->data(USER_DATA_COL, Qt::UserRole);
  return result.value<SegmentDataWrapper>();
}

void OmSegmentListWidget::setRowFlags(QTreeWidgetItem* row) {
  row->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable |
                Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
}

void OmSegmentListWidget::keyPressEvent(QKeyEvent* event) {
  QTreeWidget::keyPressEvent(event);

  switch (event->key()) {
    case Qt::Key_Up:
    case Qt::Key_Down:
      QTreeWidgetItem* current = currentItem();
      QVariant result = current->data(USER_DATA_COL, Qt::UserRole);
      SegmentDataWrapper sdw = result.value<SegmentDataWrapper>();

      OmSegmentSelector sel(sdw.MakeSegmentationDataWrapper(), this,
                            eventSenderName());
      sel.selectJustThisSegment(sdw.GetID(), true);
      break;
  }
}

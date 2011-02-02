#include "common/omCommon.h"
#include "gui/guiUtils.hpp"
#include "gui/inspectors/segObjectInspector.h"
#include "gui/segmentLists/details/segmentListBase.h"
#include "gui/widgets/omSegmentContextMenu.h"
#include "gui/widgets/omSegmentListWidget.h"
#include "segment/details/omSegmentListsTypes.hpp"
#include "segment/omSegmentSelector.h"
#include "segment/omSegmentUtils.hpp"
#include "system/omEvents.h"
#include "utility/dataWrappers.h"
#include "utility/omStringHelpers.h"
#include "viewGroup/omViewGroupState.h"
#include "zi/omUtility.h"

Q_DECLARE_METATYPE(SegmentDataWrapper);

OmSegmentListWidget::OmSegmentListWidget(SegmentListBase* slist,
										 OmViewGroupState* vgs)
	: QTreeWidget(slist)
	, segmentListBase(slist)
	, vgs_(vgs)
{
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setAlternatingRowColors(true);

	QStringList headers;
	headers << tr("ID") << tr("NumChlidren") << tr("Total Size");
	setColumnCount(headers.size());
	setHeaderLabels(headers);

	setFocusPolicy(Qt::StrongFocus);
}

bool OmSegmentListWidget::populate(const bool doScrollToSelectedSegment,
								   const OmID segmentJustSelectedID,
								   SegmentationDataWrapper segmentationDW,
								   boost::shared_ptr<OmSegIDsListWithPage> segIDs )
{
	bool makeTabActive = false;

	setUpdatesEnabled( false );
	clear();
	selectionModel()->blockSignals(true);
	selectionModel()->clearSelection();

	QTreeWidgetItem *rowToJumpTo = NULL;

	assert( 100 >= segIDs->List()->size() && "too many segments returned" );

	OmSegmentCache* segCache = segmentationDW.SegmentCache();

	FOR_EACH(iter, *segIDs->List()){
		OmSegment* seg = segCache->GetSegment(*iter);

		QTreeWidgetItem *row = new QTreeWidgetItem(this);
		setRowFlags(row);

		row->setText(ID_COL, QString::number( seg->value() ) );

		SegmentDataWrapper segDW( segmentationDW.getID(), seg->value() );
		row->setData(USER_DATA_COL, Qt::UserRole, qVariantFromValue(segDW));

		const uint32_t numChildren = OmSegmentUtils::NumberOfDescendants(segDW);
		row->setText(NUM_CHILD_COL, OmStringHelpers::CommaDeliminateNumQT(numChildren));
		row->setTextAlignment(NUM_CHILD_COL, Qt::AlignHCenter);

		row->setText(SIZE_COL, OmStringHelpers::CommaDeliminateNumQT(segDW.getSizeWithChildren()));
		row->setTextAlignment(SIZE_COL, Qt::AlignHCenter);

		row->setSelected(seg->IsSelected());
		if (doScrollToSelectedSegment && seg->value() == segmentJustSelectedID) {
			rowToJumpTo = row;
			makeTabActive = true;
		}
	}

	selectionModel()->blockSignals(false);

	GuiUtils::autoResizeColumnWidths(this, 3);

	if (doScrollToSelectedSegment && rowToJumpTo != NULL) {
		scrollToItem(rowToJumpTo);
	}

	setUpdatesEnabled(true);

	return makeTabActive;
}

std::string OmSegmentListWidget::eventSenderName()
{
	return "segmentList";
}

void OmSegmentListWidget::segmentLeftClick()
{
	segmentListBase->userJustClickedInThisSegmentList();

	QTreeWidgetItem * current = currentItem();
	if(NULL == current) {
		printf("FIXME: current segment not set\n");
		return;
	}
	QVariant result = current->data(USER_DATA_COL, Qt::UserRole);
	SegmentDataWrapper sdw = result.value < SegmentDataWrapper > ();

	OmSegmentSelector sel(sdw.MakeSegmentationDataWrapper(), this, eventSenderName() );
	sel.AddToRecentList(segmentListBase->shouldSelectedSegmentsBeAddedToRecentList());

	sel.selectNoSegments();

	foreach(QTreeWidgetItem * item, selectedItems()) {
		QVariant result = item->data(USER_DATA_COL, Qt::UserRole);
		SegmentDataWrapper item_sdw = result.value < SegmentDataWrapper > ();

		if (QApplication::keyboardModifiers() & Qt::ControlModifier ||
			QApplication::keyboardModifiers() & Qt::ShiftModifier ){
			sel.augmentSelectedSet( item_sdw.getID(), true );
		} else {
			if( selectedItems().size() > 1 ){
				if( item_sdw.getID() == sdw.getID() ){
					sel.augmentSelectedSet( item_sdw.getID(), true );
				} else {
					sel.augmentSelectedSet( item_sdw.getID(), false );
				}
			} else {
				sel.augmentSelectedSet( item_sdw.getID(), !item_sdw.isSelected() );
			}
		}
	}

	sel.AutoCenter(true);
	sel.sendEvent();
}

void OmSegmentListWidget::mousePressEvent(QMouseEvent* event)
{
	QTreeWidget::mousePressEvent(event);

	if (event->button() == Qt::LeftButton) {
		segmentLeftClick();
	} else if (event->button() == Qt::RightButton) {
		segmentRightClick(event);
	}
}

void OmSegmentListWidget::segmentRightClick(QMouseEvent* event)
{
	if( !isSegmentSelected() ){
		return;
	}

	segmentShowContexMenu(event);
}

void OmSegmentListWidget::segmentShowContexMenu(QMouseEvent* event)
{
	SegmentDataWrapper sdw = getCurrentlySelectedSegment();

	mSegmentContextMenu.Refresh(sdw, vgs_);
	mSegmentContextMenu.exec(event->globalPos());
}

bool OmSegmentListWidget::isSegmentSelected()
{
	if( NULL == currentItem() ) {
		return false;
	}
	return true;
}

SegmentDataWrapper OmSegmentListWidget::getCurrentlySelectedSegment()
{
	QTreeWidgetItem * segmentItem = currentItem();
	QVariant result = segmentItem->data(USER_DATA_COL, Qt::UserRole);
	return result.value < SegmentDataWrapper > ();
}

void OmSegmentListWidget::setRowFlags(QTreeWidgetItem * row)
{
	row->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable |
				  Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
}

void OmSegmentListWidget::keyPressEvent(QKeyEvent* event)
{
	QTreeWidget::keyPressEvent(event);

	switch (event->key()) {
	case Qt::Key_Up:
	case Qt::Key_Down:
		QTreeWidgetItem * current = currentItem();
		QVariant result = current->data(USER_DATA_COL, Qt::UserRole);
		SegmentDataWrapper sdw = result.value < SegmentDataWrapper > ();

		OmSegmentSelector sel(sdw.MakeSegmentationDataWrapper(), this, eventSenderName() );
		sel.selectJustThisSegment( sdw.getID(), true );
		break;
	}
}

#include "common/omCommon.h"
#include "gui/guiUtils.h"
#include "gui/omSegmentListWidget.h"
#include "segment/omSegmentSelector.h"
#include "utility/dataWrappers.h"
#include "inspectors/inspectorProperties.h"
#include "inspectors/segObjectInspector.h"
#include "system/viewGroup/omViewGroupState.h"
#include "gui/omSegmentContextMenu.h"

Q_DECLARE_METATYPE(SegmentDataWrapper);

OmSegmentListWidget::OmSegmentListWidget(QWidget * parent, InspectorProperties * ip) 
	: QTreeWidget(parent)
	, inspectorProperties(ip)
{
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setAlternatingRowColors(true);
	setColumnCount(3);

	QStringList headers;
	//	headers << tr("enabled") << tr("Name") << tr("ID") << tr("Notes");
	headers << tr("enabled") << tr("Name") << tr("ID");
	setHeaderLabels(headers);

	setFocusPolicy(Qt::StrongFocus);
}

void OmSegmentListWidget::populateSegmentElementsListWidget(const bool doScrollToSelectedSegment,
							    const OmId segmentJustSelectedID,
							    SegmentationDataWrapper segmentationDW,
							    OmSegPtrList * segs )
{
	setUpdatesEnabled( false );
	clear();
	selectionModel()->blockSignals(true);
	selectionModel()->clearSelection();

	QTreeWidgetItem *rowToJumpTo = NULL;

	if( 100 < segs->size() ){
		assert(0 && "too many segments returned" );
	}
	
	OmSegment * seg;
	OmSegPtrList::iterator iter;
	for( iter = segs->begin(); iter != segs->end(); ++iter){
		seg = (*iter);

		QTreeWidgetItem *row = new QTreeWidgetItem(this);
		row->setText(NAME_COL, seg->GetName());
		row->setText(ID_COL, QString::number( seg->getValue() ) );

		SegmentDataWrapper segDW( segmentationDW.getID(), seg->getValue() );
		row->setData(USER_DATA_COL, Qt::UserRole, qVariantFromValue(segDW));

		//row->setText(NOTE_COL, seg.getNote());
		setRowFlagsAndCheckState(row, GuiUtils::getCheckState(seg->IsEnabled()));
		row->setSelected(seg->IsSelected());
		if (doScrollToSelectedSegment && seg->getValue() == segmentJustSelectedID) {
			rowToJumpTo = row;
		}
	}

	selectionModel()->blockSignals(false);

	GuiUtils::autoResizeColumnWidths(this, 3);

	if (doScrollToSelectedSegment && rowToJumpTo != NULL) {
		scrollToItem(rowToJumpTo, QAbstractItemView::PositionAtCenter);
	}

	setUpdatesEnabled( true);
}

string OmSegmentListWidget::eventSenderName()
{
	return "segmentList";
}

void OmSegmentListWidget::segmentLeftClick()
{
	QTreeWidgetItem * current = currentItem();
	if(NULL == current) {
		printf("FIXME: current segment not set\n");
		return;
	}
	QVariant result = current->data(USER_DATA_COL, Qt::UserRole);
	SegmentDataWrapper sdw = result.value < SegmentDataWrapper > ();
	
	OmSegmentSelector sel(sdw.getSegmentationID(), this, eventSenderName() );

	const int column = currentColumn();
	if (0 == column) {
		const bool isChecked = GuiUtils::getBoolState( current->checkState( ENABLED_COL ) );
		sdw.setEnabled(isChecked);

		sel.selectJustThisSegment( sdw.getID(), isChecked );

		if( isChecked ) {
			setCurrentItem( current, 0, QItemSelectionModel::Select );
		} else {
			setCurrentItem( current, 0, QItemSelectionModel::Deselect );
		}

	} else {
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
	}

	sel.sendEvent();
}

void OmSegmentListWidget::mousePressEvent(QMouseEvent* event)
{
	QTreeWidget::mousePressEvent(event);

        if (event->button() == Qt::LeftButton) {
		segmentLeftClick();
	}
	if (event->button() == Qt::RightButton) {
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

        const OmId segmentationID = sdw.getSegmentation().GetId();
        const OmId segmentID = sdw.getID();

        mSegmentContextMenu.Refresh(segmentationID, segmentID, inspectorProperties->getViewGroupState());
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

void OmSegmentListWidget::setRowFlagsAndCheckState(QTreeWidgetItem * row, Qt::CheckState checkState)
{
	row->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
	row->setCheckState(ENABLED_COL, checkState);
}

void OmSegmentListWidget::addToSplitterDataElementSegment( SegmentDataWrapper sdw )
{
	assert(inspectorProperties);
	inspectorProperties->setOrReplaceWidget( new SegObjectInspector(sdw, this), 
						 QString("Segmentation%1: Segment %2")
						 .arg(sdw.getSegmentationID())
						 .arg(sdw.getID()) );
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

		OmSegmentSelector sel(sdw.getSegmentationID(), this, eventSenderName() );
		sel.selectJustThisSegment( sdw.getID(), true );
		break;
	}
}

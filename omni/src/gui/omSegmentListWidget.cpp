#include "common/omCommon.h"
#include "gui/guiUtils.h"
#include "gui/omSegmentListWidget.h"
#include "segment/omSegmentSelector.h"
#include "utility/dataWrappers.h"
#include "inspectors/inspectorProperties.h"
#include "inspectors/segObjectInspector.h"

Q_DECLARE_METATYPE(SegmentDataWrapper);

OmSegmentListWidget::OmSegmentListWidget(QWidget * parent, InspectorProperties * ip) 
	: QTreeWidget(parent)
	, inspectorProperties(ip)
{
	setupDataElementList();
}

OmSegmentListWidget::~OmSegmentListWidget()
{

}

void OmSegmentListWidget::setupDataElementList()
{
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setAlternatingRowColors(true);
	setColumnCount(3);

	QStringList headers;
	//	headers << tr("enabled") << tr("Name") << tr("ID") << tr("Notes");
	headers << tr("enabled") << tr("Name") << tr("ID");
	setHeaderLabels(headers);

	setFocusPolicy(Qt::ClickFocus);
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

	setFocusPolicy(Qt::StrongFocus);
}

void OmSegmentListWidget::segmentLeftClick()
{
	QTreeWidgetItem * current = currentItem();
	int column = currentColumn();

	QVariant result = current->data(USER_DATA_COL, Qt::UserRole);
	SegmentDataWrapper sdw = result.value < SegmentDataWrapper > ();
	
	OmSegmentSelector sel(sdw.getSegmentationID(), this, "segmentList" );

	if (0 == column) {
		const bool isChecked = GuiUtils::getBoolState( current->checkState( ENABLED_COL ) );
		sdw.setEnabled(isChecked);

		sel.selectJustThisSegment( sdw.getID(), isChecked );
		sel.sendEvent();

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
			sel.augmentSelectedSet( item_sdw.getID(), true );
		}
		sel.sendEvent();
	}
}

void OmSegmentListWidget::mousePressEvent(QMouseEvent* event)
{
	QTreeWidget::mousePressEvent(event);

        if (event->button() == Qt::LeftButton) {
		segmentLeftClick();
	}
	if (event->button() == Qt::RightButton) {
		segmentRightClick();
	}
}

void OmSegmentListWidget::segmentRightClick()
{
	if( !isSegmentSelected() ){
		return;
	}

	SegmentDataWrapper sdw = getCurrentlySelectedSegment();

	addToSplitterDataElementSegment( sdw ); 
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
	SegmentDataWrapper sdw = result.value < SegmentDataWrapper > ();
	return sdw;
}

void OmSegmentListWidget::setRowFlagsAndCheckState(QTreeWidgetItem * row, Qt::CheckState checkState)
{
	row->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
	row->setCheckState(ENABLED_COL, checkState);
}

void OmSegmentListWidget::addToSplitterDataElementSegment( SegmentDataWrapper sdw )
{
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
		// TODO: move up segment list
		break;
	case Qt::Key_Down:
		// TODO: move down segment list
		break;
	}
}

#include "segmentList.h"
#include "segment/actions/segment/omSegmentSelectAction.h"
#include "guiUtils.h"

Q_DECLARE_METATYPE(SegmentDataWrapper);

SegmentList::SegmentList( QWidget * parent, 
			  InspectorProperties * in_inspectorProperties,
			  QTabWidget * in_dataElementsTabs ) 
  : QWidget( parent )
{
	inspectorProperties = in_inspectorProperties;
	dataElementsTabs = in_dataElementsTabs;
	haveValidSDW = false;
}

void SegmentList::populateSegmentElementsListWidget(const bool doScrollToSelectedSegment,
							  const OmId segmentJustSelectedID)
{
	debug("guievent", "in %s...\n", __FUNCTION__ );

	if( !haveValidSDW ){
		printf("invalid sdw...\n");
		return;
	}

	SegmentationDataWrapper sdw = currentSDW;
	const OmId segmenID = sdw.getID();

	if (!hashOfSementationsAndSegments.contains(segmenID)) {
		hashOfSementationsAndSegments[segmenID] = sdw.getAllSegmentIDsAndNames();
	} else {
		const unsigned int num_segs_from_hash = hashOfSementationsAndSegments.value(segmenID).size();
		const unsigned int num_segs_from_core = sdw.getNumberOfSegments();
		if( num_segs_from_hash != num_segs_from_core ){
			hashOfSementationsAndSegments[segmenID] = sdw.getAllSegmentIDsAndNames();
		}
	}

	setTabEnabled( setupDataElementList(), QString("Seg%1: All Segments").arg(sdw.getID()) );
	dataElementsWidget->clear();
	dataElementsWidget->selectionModel()->blockSignals(true);
	dataElementsWidget->selectionModel()->clearSelection();

	QTreeWidgetItem *rowToJumpTo = NULL;
	QHash < OmId, SegmentDataWrapper > segs = hashOfSementationsAndSegments.value(segmenID);

	//OmSegmentation& segmentation = OmVolume::GetSegmentation( sdw.getID() );

	int count = 0;
	foreach(SegmentDataWrapper seg, segs) {
		count++;
		if (count > 60000){
			break;
		}

		QTreeWidgetItem *row = new QTreeWidgetItem(dataElementsWidget);
		row->setText(NAME_COL, seg.getName());
		row->setText(ID_COL, seg.getIDstr());
		row->setData(USER_DATA_COL, Qt::UserRole, qVariantFromValue(seg));
		row->setText(NOTE_COL, seg.getNote());
		setRowFlagsAndCheckState(row, GuiUtils::getCheckState(seg.isEnabled()));
		row->setSelected(seg.isSelected());
		if (doScrollToSelectedSegment && seg.getID() == segmentJustSelectedID) {
			rowToJumpTo = row;
		}
	}

	dataElementsWidget->selectionModel()->blockSignals(false);
	dataElementsWidget->update();

	dataElementsWidget->disconnect(SIGNAL(itemClicked(QTreeWidgetItem *, int)));
	connect(dataElementsWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
		this, SLOT(leftClickOnSegment(QTreeWidgetItem *, int)));

	GuiUtils::autoResizeColumnWidths(dataElementsWidget, 3);

	if (doScrollToSelectedSegment && rowToJumpTo != NULL) {
		dataElementsWidget->scrollToItem(rowToJumpTo, QAbstractItemView::PositionAtCenter);
	}
}

void SegmentList::leftClickOnSegment(QTreeWidgetItem * current, const int column)
{
	addToSplitterDataElementSegment(current, column);

	QVariant result = current->data(USER_DATA_COL, Qt::UserRole);
	SegmentDataWrapper sdw = result.value < SegmentDataWrapper > ();

	// TODO: make sure list of modified segments is correct....

	if (0 == column) {
		const bool isChecked = GuiUtils::getBoolState( current->checkState( ENABLED_COL ) );
		sdw.setEnabled(isChecked);
		sendSegmentChangeEvent(sdw, false);
		dataElementsWidget->setCurrentItem( current, 0, QItemSelectionModel::Select );
	} else {
		OmSegmentation & segmentation = OmVolume::GetSegmentation(sdw.getSegmentationID());
		segmentation.SetAllSegmentsSelected(false);
		
		foreach(QTreeWidgetItem * item, dataElementsWidget->selectedItems()) {
			QVariant result = item->data(USER_DATA_COL, Qt::UserRole);
			SegmentDataWrapper item_sdw = result.value < SegmentDataWrapper > ();
			item_sdw.setSelected(true);
		}
		sendSegmentChangeEvent(sdw, true);
	}
}

void SegmentList::addToSplitterDataElementSegment(QTreeWidgetItem * current, const int column)
{
	QVariant result = current->data(USER_DATA_COL, Qt::UserRole);
	SegmentDataWrapper sdw = result.value < SegmentDataWrapper > ();

	segObjectInspectorWidget = new SegObjectInspector(sdw, this);

	inspectorProperties->setOrReplaceWidget( segObjectInspectorWidget, 
						 QString("Segmentation%1: Segment %2")
						 .arg(sdw.getSegmentationID())
						 .arg(sdw.getID()) );
}

void SegmentList::sendSegmentChangeEvent(SegmentDataWrapper sdw, const bool augment_selection)
{
	const OmId segmentationID = sdw.getSegmentationID();
	const OmId segmentID = sdw.getID();
	OmSegmentation & segmentation = OmVolume::GetSegmentation(segmentationID);

	OmIds selected_segment_ids;
	OmIds un_selected_segment_ids;

	if (augment_selection) {
		selected_segment_ids = segmentation.GetSelectedSegmentIds();
	} else {
		selected_segment_ids.insert(segmentID);
		un_selected_segment_ids = segmentation.GetSelectedSegmentIds();
		un_selected_segment_ids.erase(segmentID);
	}

	(new OmSegmentSelectAction(segmentationID,
				   selected_segment_ids, 
				   un_selected_segment_ids, 
				   segmentID, 
				   this,
				   "segmentList"))->Run();
}

QTreeWidget * SegmentList::setupDataElementList()
{
	dataElementsWidget = new QTreeWidget(this);
	dataElementsWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	dataElementsWidget->setAlternatingRowColors(true);
	dataElementsWidget->setColumnCount(3);

	QStringList headers;
	headers << tr("enabled") << tr("Name") << tr("ID") << tr("Notes");
	dataElementsWidget->setHeaderLabels(headers);

	return dataElementsWidget;
}

void SegmentList::setTabEnabled( QWidget * tab, QString title )
{
	dataElementsTabs->removeTab( 0 );
	dataElementsTabs->addTab( tab, title );	
}

void SegmentList::setRowFlagsAndCheckState(QTreeWidgetItem * row, Qt::CheckState checkState)
{
	row->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
	row->setCheckState(ENABLED_COL, checkState);
}

void SegmentList::makeSegmentationActive(const OmId segmentationID)
{
	currentSDW = SegmentationDataWrapper(segmentationID);
	haveValidSDW = true;
	populateSegmentElementsListWidget();
}

void SegmentList::makeSegmentationActive(SegmentationDataWrapper sdw)
{
	currentSDW = sdw;
	haveValidSDW = true;
	populateSegmentElementsListWidget();
}

void SegmentList::makeSegmentationActive(const OmId segmentationID, const OmId segmentJustSelectedID)
{
	currentSDW = SegmentationDataWrapper(segmentationID);
	haveValidSDW = true;
	populateSegmentElementsListWidget(true, segmentJustSelectedID);
}

void SegmentList::makeSegmentationActive(SegmentationDataWrapper sdw, const OmId segmentJustSelectedID)
{
	currentSDW = sdw;
	haveValidSDW = true;
	populateSegmentElementsListWidget(true, segmentJustSelectedID);
}

void SegmentList::rebuildSegmentList(const OmId segmentationID)
{
	hashOfSementationsAndSegments.remove(segmentationID);
	makeSegmentationActive(segmentationID);
}

void SegmentList::rebuildSegmentList(const OmId segmentationID,
					   const OmId segmentJustAddedID)
{
	hashOfSementationsAndSegments.remove(segmentationID);
	makeSegmentationActive(segmentationID, segmentJustAddedID );
}



/*

 protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);


void MyInspectorWidget::mousePressEvent(QMouseEvent *event)
{
	debug("guimouse", "mouse start\n");
}

void MyInspectorWidget::mouseMoveEvent(QMouseEvent *event)
{
	debug("guimouse", "mouse move\n");
}

void MyInspectorWidget::mouseReleaseEvent(QMouseEvent *event)
{
	debug("guimouse", "mouse release\n");
}
*/
/*
        switch (event->key()) {
        case Qt::Key_Up:
                debug("gui", "hi key up\n");
                break;
        case Qt::Key_Down:
                debug("gui", "hi key down\n");
                break;
        }       
*/

void SegmentList::dealWithSegmentObjectModificationEvent(OmSegmentEvent * event)
{
	// quick hack; assumes userData is pointer to sender (and we're the only
	//  ones to set the sender...)
	if (this == event->getSender()) {
		debug("guievent", "in SegmentList:%s: i sent it! (%s)\n", __FUNCTION__, event->getComment().c_str());
		return;
	} else {
		debug("guievent", "in SegmentList:%s: i did NOT send it! (%s)\n", __FUNCTION__, event->getComment().c_str());
	}

	const OmId segmentationID = event->GetModifiedSegmentationId();
	if (!OmVolume::IsSegmentationValid(segmentationID)) {
		if( haveValidSDW ){
			populateSegmentElementsListWidget();
		}
		return;
	}

	OmIds selection_changed_segmentIDs = event->GetModifiedSegmentIds();
	const OmId segmentJustSelectedID = event->GetSegmentJustSelectedID();

	makeSegmentationActive(segmentationID, segmentJustSelectedID);
}

#include "project/omProject.h"
#include "gui/segmentList.h"
#include "segment/omSegmentSelector.h"
#include "gui/guiUtils.h"
#include "volume/omSegmentation.h"
#include "segment/omSegmentCache.h"


Q_DECLARE_METATYPE(SegmentDataWrapper);

SegmentList::SegmentList( QWidget * parent, 
			  InspectorProperties * in_inspectorProperties,
			  ElementListBox * in_elementListBox ) 
	: QWidget( parent )
	, dataElementsWidget( NULL )
	, inspectorProperties( in_inspectorProperties )
	, elementListBox( in_elementListBox )
	, haveValidSDW( false )
	, currentPageNum( 0 )
{
}

int SegmentList::getNumSegmentsPerPage()
{
	return 100;
}

quint32 SegmentList::getMaxSegmentValue()
{
	assert( haveValidSDW );
	return currentSDW.getMaxSegmentValue();
}

boost::shared_ptr<OmSegIDs> SegmentList::getSegmentsToDisplay( const OmId firstSegmentID )
{
	int offset = firstSegmentID - (firstSegmentID % getNumSegmentsPerPage() );
	return doGetSegmentsToDisplay( offset );
}

boost::shared_ptr<OmSegIDs> SegmentList::doGetSegmentsToDisplay( const unsigned int in_offset )
{
	assert( haveValidSDW );

	unsigned int offset = 0;
	if( getMaxSegmentValue() > in_offset ){
		offset = in_offset;
	}

	OmSegIDs * ret = currentSDW.getSegmentCache()->getRootLevelSegIDs( offset, getNumSegmentsPerPage() );
	
	return boost::shared_ptr<OmSegIDs>( ret );
}

void SegmentList::populateSegmentElementsListWidget(const bool doScrollToSelectedSegment,
						    const OmId segmentJustSelectedID)
{
	debug("guievent", "in %s...\n", __FUNCTION__ );

	if( !haveValidSDW ){
		printf("invalid sdw...\n");
		return;
	}
	if( NULL == dataElementsWidget ){
		setupDataElementList();
	}

	SegmentationDataWrapper sdw = currentSDW;
	boost::shared_ptr<OmSegIDs> segs = getSegmentsToDisplay( segmentJustSelectedID );

	dataElementsWidget->setUpdatesEnabled( false );
	dataElementsWidget->clear();
	dataElementsWidget->selectionModel()->blockSignals(true);
	dataElementsWidget->selectionModel()->clearSelection();

	QTreeWidgetItem *rowToJumpTo = NULL;

	foreach(OmSegID segID, (*segs)) {
		SegmentDataWrapper seg(  sdw.getID(), segID );

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

	dataElementsWidget->disconnect(SIGNAL(itemClicked(QTreeWidgetItem *, int)));
	connect(dataElementsWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
		this, SLOT(leftClickOnSegment(QTreeWidgetItem *, int)));

	dataElementsWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(dataElementsWidget, SIGNAL(customContextMenuRequested(const QPoint &)),
		this, SLOT(showContextMenu(const QPoint &)));

	GuiUtils::autoResizeColumnWidths(dataElementsWidget, 3);

	if (doScrollToSelectedSegment && rowToJumpTo != NULL) {
		dataElementsWidget->scrollToItem(rowToJumpTo, QAbstractItemView::PositionAtCenter);
	}

	dataElementsWidget->setUpdatesEnabled( true);

	elementListBox->setTabEnabled( QString("Segmentation %1").arg(sdw.getID()),
				       dataElementsWidget,
				       QString("All Segments") );
	dealWithButtons();

	setFocusPolicy(Qt::StrongFocus);
}

void SegmentList::dealWithButtons()
{
	elementListBox->prevButton->disconnect(SIGNAL( released() ));
	connect( elementListBox->prevButton, SIGNAL( released()  ), 
		 this, SLOT( goToPrevPage() ), Qt::DirectConnection);

	elementListBox->nextButton->disconnect(SIGNAL( released() ));
	connect( elementListBox->nextButton, SIGNAL( released()  ), 
		 this, SLOT( goToNextPage() ), Qt::DirectConnection);
}

void SegmentList::goToNextPage()
{
	currentPageNum++;
	unsigned int offset = currentPageNum * getNumSegmentsPerPage();
	if( offset > getMaxSegmentValue() ){
		currentPageNum--;
		offset = currentPageNum * getNumSegmentsPerPage();
	}
	populateSegmentElementsListWidget( false, offset );
}

void SegmentList::goToPrevPage()
{
	currentPageNum--;
	if( currentPageNum < 0 ){
		currentPageNum = 0;
	}
	int offset = currentPageNum * getNumSegmentsPerPage();
	populateSegmentElementsListWidget( false, offset );
}

void SegmentList::showContextMenu(const QPoint & menuPoint)
{
	QTreeWidgetItem * segmentItem = dataElementsWidget->itemAt(menuPoint);
	if (!segmentItem) {	// right click occured in "white space" of widget
		/*
		connect(makeSegmentContextMenu(dataElementsWidget), SIGNAL(triggered(QAction *)), 
			this, SLOT(doDataSrcContextMenuVolAdd(QAction *)));

		contextMenu->exec(dataElementsWidget->mapToGlobal(menuPoint));
		*/
		return;
	}

	showSegmentContextMenu();
}

void SegmentList::showSegmentContextMenu()
{
	connect(makeSegmentContextMenu(dataElementsWidget), SIGNAL(triggered(QAction *)), 
		this, SLOT(segmentRightClickMenu(QAction *)));

	contextMenu->exec(QCursor::pos());
}

void SegmentList::segmentRightClickMenu(QAction * act)
{
	if( !isSegmentSelected() ){
		return;
	}
	SegmentDataWrapper sdw = getCurrentlySelectedSegment();
	if( propAct == act ){
		addToSplitterDataElementSegment( sdw );
	} 
}

QMenu * SegmentList::makeSegmentContextMenu(QTreeWidget * parent)
{
	propAct = new QAction(tr("&Properties"), parent);
	contextMenu = new QMenu(parent);
	contextMenu->addAction(propAct);

	return contextMenu;
}

void SegmentList::leftClickOnSegment(QTreeWidgetItem * current, const int column)
{
	if (QApplication::keyboardModifiers() & Qt::AltModifier ||
	    inspectorProperties->isVisible() ) {
		if( isSegmentSelected() ){
			SegmentDataWrapper sdw = getCurrentlySelectedSegment();
			addToSplitterDataElementSegment(sdw);
		}
	}

	QVariant result = current->data(USER_DATA_COL, Qt::UserRole);
	SegmentDataWrapper sdw = result.value < SegmentDataWrapper > ();
	
	OmSegmentSelector sel(sdw.getSegmentationID(), this, "segmentList" );

	if (0 == column) {
		const bool isChecked = GuiUtils::getBoolState( current->checkState( ENABLED_COL ) );
		sdw.setEnabled(isChecked);

		sel.selectJustThisSegment( sdw.getID(), isChecked );
		sel.sendEvent();

		if( isChecked ) {
			dataElementsWidget->setCurrentItem( current, 0, QItemSelectionModel::Select );
		} else {
			dataElementsWidget->setCurrentItem( current, 0, QItemSelectionModel::Deselect );
		}
	} else {
		sel.selectNoSegments();

		foreach(QTreeWidgetItem * item, dataElementsWidget->selectedItems()) {
			QVariant result = item->data(USER_DATA_COL, Qt::UserRole);
			SegmentDataWrapper item_sdw = result.value < SegmentDataWrapper > ();
			sel.augmentSelectedSet( item_sdw.getID(), true );
		}
		sel.sendEvent();
	}
}

void SegmentList::addToSplitterDataElementSegment( SegmentDataWrapper sdw )
{
	segObjectInspectorWidget = new SegObjectInspector(sdw, this);

	inspectorProperties->setOrReplaceWidget( segObjectInspectorWidget, 
						 QString("Segmentation%1: Segment %2")
						 .arg(sdw.getSegmentationID())
						 .arg(sdw.getID()) );
}

void SegmentList::setupDataElementList()
{
	dataElementsWidget = new QTreeWidget(this);
	dataElementsWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	dataElementsWidget->setAlternatingRowColors(true);
	dataElementsWidget->setColumnCount(3);

	QStringList headers;
	headers << tr("enabled") << tr("Name") << tr("ID") << tr("Notes");
	dataElementsWidget->setHeaderLabels(headers);

	dataElementsWidget->setFocusPolicy(Qt::ClickFocus);
}

void SegmentList::setRowFlagsAndCheckState(QTreeWidgetItem * row, Qt::CheckState checkState)
{
	row->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
	row->setCheckState(ENABLED_COL, checkState);
}

void SegmentList::makeSegmentationActive(const OmId segmentationID)
{
	makeSegmentationActive( SegmentationDataWrapper(segmentationID) );
}

void SegmentList::makeSegmentationActive(SegmentationDataWrapper sdw)
{
	currentSDW = sdw;
	haveValidSDW = true;
	populateSegmentElementsListWidget();
}

void SegmentList::makeSegmentationActive(const OmId segmentationID, const OmId segmentJustSelectedID)
{
	makeSegmentationActive( SegmentationDataWrapper(segmentationID), segmentJustSelectedID);
}

void SegmentList::makeSegmentationActive(SegmentationDataWrapper sdw, const OmId segmentJustSelectedID)
{
	currentSDW = sdw;
	haveValidSDW = true;
	populateSegmentElementsListWidget(true, segmentJustSelectedID);
}

void SegmentList::rebuildSegmentList(const OmId segmentationID)
{
	makeSegmentationActive(segmentationID);
}

void SegmentList::rebuildSegmentList(const OmId segmentationID,
					   const OmId segmentJustAddedID)
{
	makeSegmentationActive(segmentationID, segmentJustAddedID );
}

void SegmentList::keyPressEvent(QKeyEvent * event)
{
	printf("hi\n");

	switch (event->key()) {
	case Qt::Key_Up:
		printf("hi from keyup\n");
		break;
	case Qt::Key_Down:
		printf("hi from keyup\n");
		break;
	}
}

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
	if (!OmProject::IsSegmentationValid(segmentationID)) {
		if( haveValidSDW ){
			populateSegmentElementsListWidget();
		}
		return;
	}

	const OmId segmentJustSelectedID = event->GetSegmentJustSelectedID();

	makeSegmentationActive(segmentationID, segmentJustSelectedID);
}

bool SegmentList::isSegmentSelected()
{
	if( NULL == dataElementsWidget->currentItem() ) {
		return false;
	}
	return true;
}

SegmentDataWrapper SegmentList::getCurrentlySelectedSegment()
{
	QTreeWidgetItem * segmentItem = dataElementsWidget->currentItem();
	QVariant result = segmentItem->data(USER_DATA_COL, Qt::UserRole);
	SegmentDataWrapper sdw = result.value < SegmentDataWrapper > ();
	return sdw;
}

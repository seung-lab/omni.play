#include <QtGui>
#include "myInspectorWidget.h"
#include "guiUtils.h"

#include "volume/omVolumeTypes.h"
#include "common/omStd.h"
#include "project/omProject.h"
#include "system/omSystemTypes.h"
#include "system/omManageableObject.h"
#include "volume/omVolume.h"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"
#include "segment/omSegment.h"

#include "segment/actions/segment/omSegmentSelectAction.h"

#include "system/omEventManager.h"
#include "system/events/omView3dEvent.h"
#include "system/events/omViewEvent.h"

#include <vmmlib/vmmlib.h>
#include <vmmlib/serialization.h>
#include "common/omDebug.h"
using namespace vmml;

Q_DECLARE_METATYPE(DataWrapperContainer);
Q_DECLARE_METATYPE(SegmentDataWrapper);
Q_DECLARE_METATYPE(FilterDataWrapper);

MyInspectorWidget::MyInspectorWidget(QWidget * parent):QWidget(parent)
{
	layoutWidget = new QWidget( this );
	QVBoxLayout *verticalLayout = new QVBoxLayout(layoutWidget);
	verticalLayout->addWidget(setupVolumeList(layoutWidget));
	verticalLayout->addWidget(setupDataSrcList());
	dataElementsTabs = new QTabWidget();
	verticalLayout->addWidget(dataElementsTabs);

	currentDataSrc = DataWrapperContainer();
	inspectorProperties = new InspectorProperties( this );

	QMetaObject::connectSlotsByName(this);
}

MyInspectorWidget::~MyInspectorWidget()
{
}

void MyInspectorWidget::setTabEnabled( QWidget * tab, QString title )
{
	dataElementsTabs->removeTab( 0 );
	dataElementsTabs->addTab( tab, title );	
}

QTreeWidget *MyInspectorWidget::setupDataElementList()
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

QTreeWidget *MyInspectorWidget::setupFilterList()
{
	filterListWidget = new QTreeWidget(this);
	filterListWidget->setAlternatingRowColors(false);
	filterListWidget->setColumnCount(3);

	QStringList headers;
	headers << tr("enabled") << tr("Name") << tr("ID") << tr("Notes");
	filterListWidget->setHeaderLabels(headers);

	return filterListWidget;
}

QTreeWidget *MyInspectorWidget::setupDataSrcList()
{
	dataSrcListWidget = new QTreeWidget(this);
	dataSrcListWidget->setAlternatingRowColors(false);
	dataSrcListWidget->setColumnCount(3);
	QStringList headers;
	headers << tr("enabled") << tr("Name") << tr("ID") << tr("Notes");
	dataSrcListWidget->setHeaderLabels(headers);

	populateDataSrcListWidget();

	// context menu setup
	dataSrcListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(dataSrcListWidget, SIGNAL(customContextMenuRequested(const QPoint &)),
		this, SLOT(showDataSrcContextMenu(const QPoint &)));

	return dataSrcListWidget;
}

QTreeWidget *MyInspectorWidget::setupVolumeList(QWidget * layoutWidget)
{
	QTreeWidget *volumeListWidget = new QTreeWidget(layoutWidget);
	volumeListWidget->setMaximumSize(QSize(16777215, 75));
	volumeListWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	volumeListWidget->setHeaderHidden(true);

	return volumeListWidget;
}

void MyInspectorWidget::setRowFlagsAndCheckState(QTreeWidgetItem * row, Qt::CheckState checkState)
{
	row->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
	row->setCheckState(ENABLED_COL, checkState);
}

void MyInspectorWidget::populateDataSrcListWidget()
{
	dataSrcListWidget->clear();

	foreach(OmId channID, OmVolume::GetValidChannelIds()) {
		DataWrapperContainer dwc = DataWrapperContainer(CHANNEL, channID);
		ChannelDataWrapper cdw = dwc.getChannelDataWrapper();
		QTreeWidgetItem *row = new QTreeWidgetItem(dataSrcListWidget);
		row->setText(NAME_COL, cdw.getName());
		row->setText(ID_COL, QString("%1").arg(cdw.getID()));
		row->setText(NOTE_COL, cdw.getNote());
		row->setData(USER_DATA_COL, Qt::UserRole, qVariantFromValue(dwc));
		setRowFlagsAndCheckState(row, GuiUtils::getCheckState(cdw.isEnabled()));
	}

	foreach(OmId segmenID, OmVolume::GetValidSegmentationIds()) {
		DataWrapperContainer dwc = DataWrapperContainer(SEGMENTATION, segmenID);
		SegmentationDataWrapper sdw = dwc.getSegmentationDataWrapper();
		QTreeWidgetItem *row = new QTreeWidgetItem(dataSrcListWidget);
		row->setText(NAME_COL, sdw.getName());
		row->setText(ID_COL, QString("%1").arg(sdw.getID()));
		row->setText(NOTE_COL, sdw.getNote());
		row->setData(USER_DATA_COL, Qt::UserRole, qVariantFromValue(dwc));
		setRowFlagsAndCheckState(row, GuiUtils::getCheckState(sdw.isEnabled()));
	}

	dataSrcListWidget->disconnect(SIGNAL(itemClicked(QTreeWidgetItem *, int)));
	connect(dataSrcListWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
		this, SLOT(leftClickOnDataSourceItem(QTreeWidgetItem *, int)));

	autoResizeColumnWidths(dataSrcListWidget, 3);

	dataSrcListWidget->update();
}

void MyInspectorWidget::populateFilterListWidget(ChannelDataWrapper cdw)
{
	setTabEnabled( setupFilterList(), "Filters" );

	filterListWidget->clear();

	const OmId segmenID = cdw.getID();

	filterListWidget->selectionModel()->blockSignals(true);
	filterListWidget->selectionModel()->clearSelection();

	foreach(FilterDataWrapper filter, cdw.getAllFilterIDsAndNames()) {
		QTreeWidgetItem *row = new QTreeWidgetItem(filterListWidget);
		row->setText(NAME_COL, filter.getName());
		row->setText(ID_COL, QString("%1").arg(filter.getID()));
		row->setData(USER_DATA_COL, Qt::UserRole, qVariantFromValue(filter));
		//row->setText( NOTE_COL, filter.getNote() );
		row->setText( NOTE_COL, "" );
		setRowFlagsAndCheckState(row, GuiUtils::getCheckState(true));
		//row->setSelected( seg.isSelected() );
	}

	filterListWidget->selectionModel()->blockSignals(false);

	filterListWidget->disconnect(SIGNAL(itemClicked(QTreeWidgetItem *, int)));
	connect(filterListWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
		this, SLOT(addToSplitterDataElementFilter(QTreeWidgetItem *, int)));

	autoResizeColumnWidths(filterListWidget, 3);

	filterListWidget->update();
}

void MyInspectorWidget::autoResizeColumnWidths(QTreeWidget * widget, const int max_col_to_display)
{
	for (int i = 0; i <= max_col_to_display; i++) {
		widget->resizeColumnToContents(i);
	}
}

void MyInspectorWidget::addChannelToSplitter(ChannelDataWrapper cdw)
{
	channelInspectorWidget = new ChanInspector( cdw, this);
	
	connect(channelInspectorWidget->addFilterButton, SIGNAL(clicked()), 
		this, SLOT(addFilter()));
	
	populateChannelInspector( cdw.getID() );

	connect(channelInspectorWidget->nameEdit, SIGNAL(editingFinished()),
		this, SLOT(nameEditChanged()), Qt::DirectConnection);

	inspectorProperties->setOrReplaceWidget( channelInspectorWidget, 
						 QString("Channel %1 Inspector").arg(cdw.getID()) );
}

void MyInspectorWidget::addSegmentationToSplitter(SegmentationDataWrapper sdw)
{
	segInspectorWidget = new SegInspector( sdw, this);
	
	connect(segInspectorWidget, SIGNAL(segmentationBuilt(OmId)), this, SLOT(rebuildSegmentList(OmId)));
	
	connect(segInspectorWidget->addSegmentButton, SIGNAL(clicked()), this, SLOT(addSegment()));

	connect(segInspectorWidget->nameEdit, SIGNAL(editingFinished()),
		this, SLOT(nameEditChanged()), Qt::DirectConnection);

	inspectorProperties->setOrReplaceWidget( segInspectorWidget, 
						 QString("Segmentation %1 Inspector").arg(sdw.getID()) );
}

void MyInspectorWidget::addToSplitterDataElementSegment(QTreeWidgetItem * current, const int column)
{
	QVariant result = current->data(USER_DATA_COL, Qt::UserRole);
	SegmentDataWrapper sdw = result.value < SegmentDataWrapper > ();

	segObjectInspectorWidget = new SegObjectInspector(sdw, this);

	inspectorProperties->setOrReplaceWidget( segObjectInspectorWidget, 
						 QString("Segment %1 Inspector").arg(sdw.getID()) );
}

void MyInspectorWidget::addToSplitterDataElementFilter(QTreeWidgetItem * current, const int column)
{
	QVariant result = current->data(USER_DATA_COL, Qt::UserRole);
	FilterDataWrapper fdw = result.value < FilterDataWrapper > ();

	filObjectInspectorWidget = new FilObjectInspector(this, fdw);

	inspectorProperties->setOrReplaceWidget( filObjectInspectorWidget, 
						 QString("Filter %1 Inspector").arg(fdw.getID()) );
}

void MyInspectorWidget::addToSplitterDataSource(QTreeWidgetItem * current, const int column)
{
	QVariant result = current->data(USER_DATA_COL, Qt::UserRole);
	DataWrapperContainer dwc = result.value < DataWrapperContainer > ();

	switch (dwc.getType()) {
	case CHANNEL:
		addChannelToSplitter(dwc.getChannelDataWrapper());
		populateFilterListWidget(dwc.getChannelDataWrapper());
		break;
	case SEGMENTATION:
		addSegmentationToSplitter(dwc.getSegmentationDataWrapper());
		makeSegmentationActive(dwc.getSegmentationDataWrapper());
		break;
	}
}

void MyInspectorWidget::showChannelContextMenu()
{
	connect(makeContextMenuBase(dataSrcListWidget),
		SIGNAL(triggered(QAction *)), this, SLOT(selectChannelView(QAction *)));

	contextMenu->exec(QCursor::pos());
}

void MyInspectorWidget::showSegmentationContextMenu()
{
	connect(makeContextMenuBase(dataSrcListWidget),
		SIGNAL(triggered(QAction *)), this, SLOT(selectSegmentationView(QAction *)));

	contextMenu->exec(QCursor::pos());
}

QMenu *MyInspectorWidget::makeDataSrcContextMenu(QTreeWidget * parent)
{
	addChannelAct = new QAction(tr("Add Channel"), parent);

	addSegmentationAct = new QAction(tr("Add Segmentation"), parent);

	contextMenuDataSrc = new QMenu(parent);
	contextMenuDataSrc->addAction(addChannelAct);
	contextMenuDataSrc->addAction(addSegmentationAct);

	return contextMenuDataSrc;
}

QMenu *MyInspectorWidget::makeContextMenuBase(QTreeWidget * parent)
{
	xyAct = new QAction(tr("&View XY"), parent);
	xyAct->setStatusTip(tr("Opens the XY view"));

	xzAct = new QAction(tr("&View XZ"), parent);
	xzAct->setStatusTip(tr("Opens the XZ view"));

	yzAct = new QAction(tr("&View YZ"), parent);
	yzAct->setStatusTip(tr("Opens the YZ view"));

	contextMenu = new QMenu(parent);
	contextMenu->addAction(xyAct);
	contextMenu->addAction(xzAct);
	contextMenu->addAction(yzAct);

	return contextMenu;
}

void MyInspectorWidget::addChannelToVolume()
{
	OmChannel & added_channel = OmVolume::AddChannel();
	addToVolume(&added_channel, CHANNEL);
}

void MyInspectorWidget::addSegmentationToVolume()
{
	OmSegmentation & added_segmentation = OmVolume::AddSegmentation();
	addToVolume(&added_segmentation, SEGMENTATION);
	makeSegmentationActive(added_segmentation.GetId());
}

void MyInspectorWidget::doDataSrcContextMenuVolAdd(QAction * act)
{
	if (act == addChannelAct) {
		addChannelToVolume();
	} else if (act == addSegmentationAct) {
		addSegmentationToVolume();
	} else {
		throw OmFormatException("could not match QAction type...\n");
	}
}

void MyInspectorWidget::showDataSrcContextMenu(const QPoint & menuPoint)
{
	QTreeWidgetItem *dataSrcItem = dataSrcListWidget->itemAt(menuPoint);
	if (!dataSrcItem) {	// right click occured in "white space" of widget (not on actual item, like channel1, etc)
		connect(makeDataSrcContextMenu(dataSrcListWidget),
			SIGNAL(triggered(QAction *)), this, SLOT(doDataSrcContextMenuVolAdd(QAction *)));

		contextMenuDataSrc->exec(dataSrcListWidget->mapToGlobal(menuPoint));
		return;
	}
	doShowDataSrcContextMenu( dataSrcItem );
}

void MyInspectorWidget::leftClickOnFilterItem(QTreeWidgetItem * current, const int column)
{
	addToSplitterDataSource(current, column );
}

void MyInspectorWidget::leftClickOnDataSourceItem(QTreeWidgetItem * current, const int column)
{
	if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
		doShowDataSrcContextMenu( current );
	} else {
		addToSplitterDataSource(current, column );
	}
}

void MyInspectorWidget::doShowDataSrcContextMenu( QTreeWidgetItem *dataSrcItem )
{
	QVariant result = dataSrcItem->data(USER_DATA_COL, Qt::UserRole);
	DataWrapperContainer dwc = result.value < DataWrapperContainer > ();

	switch (dwc.getType()) {
	case CHANNEL:
		showChannelContextMenu();
		break;
	case SEGMENTATION:
		showSegmentationContextMenu();
		makeSegmentationActive(dwc.getSegmentationDataWrapper());
		break;
	}
}

ViewType MyInspectorWidget::getViewType(QAction * act)
{
	if (act == xyAct) {
		return XY_VIEW;
	} else if (act == xzAct) {
		return XZ_VIEW;
	} else if (act == yzAct) {
		return YZ_VIEW;
	} else {
		throw OmFormatException("could not match QAction type...\n");
	}
}

void MyInspectorWidget::selectChannelView(QAction * act)
{
	OmId primary_id = getCurrentlySelectedChannel().getID();
	emit triggerChannelView(primary_id, 0, 0, getViewType(act));
}

ChannelDataWrapper MyInspectorWidget::getCurrentlySelectedChannel()
{
	QTreeWidgetItem *dataSrcItem = dataSrcListWidget->currentItem();
	QVariant result = dataSrcItem->data(USER_DATA_COL, Qt::UserRole);
	DataWrapperContainer dwc = result.value < DataWrapperContainer > ();
	return dwc.getChannelDataWrapper();
}

void MyInspectorWidget::selectSegmentationView(QAction * act)
{
	QTreeWidgetItem *dataSrcItem = dataSrcListWidget->currentItem();
	QVariant result = dataSrcItem->data(USER_DATA_COL, Qt::UserRole);
	DataWrapperContainer dwc = result.value < DataWrapperContainer > ();
	SegmentationDataWrapper sdw = dwc.getSegmentationDataWrapper();
	makeSegmentationActive(sdw);

	const OmId segmentationID = sdw.getID();
	emit triggerSegmentationView(segmentationID, 0, getViewType(act));
}

// called from mainwindow after segmentation or channel added
// "refresh datasrc list"
void MyInspectorWidget::addToVolume(OmManageableObject * item, ObjectType item_type)
{
	populateDataSrcListWidget();
}

void MyInspectorWidget::addFilter()
{
	ChannelDataWrapper cdw = channelInspectorWidget->getChannelDataWrapper();
	OmFilter2d & added_filter = OmVolume::GetChannel( cdw.getID() ).AddFilter();
	populateDataSrcListWidget();
	populateFilterListWidget(cdw);
}

void MyInspectorWidget::nameEditChanged()
{
	printf("FIXME: purcaro: sourceEditChangedSeg\n");
	/*
	   QVariant result = proxyModel->data(view->currentIndex(), Qt::UserRole);
	   int item_type = result.value<int>();

	   if(item_type == CHANNEL)
	   proxyModel->setData(view->currentIndex(), QVariant(channelInspectorWidget->nameEdit->text()), Qt::EditRole);
	   else if(item_type == SEGMENTATION)
	   proxyModel->setData(view->currentIndex(), QVariant(segInspectorWidget->nameEdit->text()), Qt::EditRole);
	   else if(item_type == SEGMENT)
	   proxyModel->setData(view->currentIndex(), QVariant(segObjectInspectorWidget->nameEdit->text()), Qt::EditRole);
	   else if(FILTER == item_type)
	   proxyModel->setData(view->currentIndex(), QVariant(filObjectInspectorWidget->nameEdit->text()), Qt::EditRole);
	 */
}

void MyInspectorWidget::populateChannelInspector(OmId c_id)
{
	OmChannel & current_channel = OmVolume::GetChannel(c_id);

	const string & my_name = current_channel.GetName();
	channelInspectorWidget->nameEdit->setText(QString::fromStdString(my_name));
	channelInspectorWidget->nameEdit->setMinimumWidth(200);

	const string & my_directory = current_channel.GetSourceDirectoryPath();
	channelInspectorWidget->directoryEdit->setText(QString::fromStdString(my_directory));
	channelInspectorWidget->directoryEdit->setMinimumWidth(200);

	////debug("genone","SOURCE DIRECTORY PATH = " << my_directory);

	const string & my_pattern = current_channel.GetSourceFilenameRegex();
	channelInspectorWidget->patternEdit->setText(QString::fromStdString(my_pattern));
	channelInspectorWidget->patternEdit->setMinimumWidth(200);

	channelInspectorWidget->listWidget->clear();
	list < string >::const_iterator match_it;
	const list < string > &matches = OmVolume::GetChannel(c_id).GetSourceFilenameRegexMatches();

	for (match_it = matches.begin(); match_it != matches.end(); ++match_it) {
		channelInspectorWidget->listWidget->addItem(QString::fromStdString(*match_it));
	}

	const string & my_notes = current_channel.GetNote();
	channelInspectorWidget->notesEdit->setPlainText(QString::fromStdString(my_notes));
}

void MyInspectorWidget::addChildrenToSegmentation(OmId seg_id)
{
	//debug("FIXME", << "MyInspectorWidget::addChildrenToSegmentation: mesh was built; should we do something? (purcaro)\n";
}

void MyInspectorWidget::SegmentObjectModificationEvent(OmSegmentEvent * event)
{
	// quick hack; assumes userData is pointer to sender (and we're the only
	//  ones to set the sender...)
	if (this == event->getSender()) {
		debug("guievent", "in MyInspectorWidget:%s: i sent it! (%s)\n", __FUNCTION__, event->getComment().c_str());
		return;
	} else {
		debug("guievent", "in MyInspectorWidget:%s: i did NOT send it! (%s)\n", __FUNCTION__, event->getComment().c_str());
	}

	const OmId segmentationID = event->GetModifiedSegmentationId();
	if (!OmVolume::IsSegmentationValid(segmentationID)) {
		if( currentDataSrc.isValidContainer() ){
			populateSegmentElementsListWidget();
		}
		return;
	}

	OmIds selection_changed_segmentIDs = event->GetModifiedSegmentIds();
	const OmId segmentJustSelectedID = event->GetSegmentJustSelectedID();

	makeSegmentationActive(segmentationID, segmentJustSelectedID);
}

void MyInspectorWidget::makeSegmentationActive(const OmId segmentationID)
{
	currentDataSrc = DataWrapperContainer(SEGMENTATION, segmentationID);
	populateSegmentElementsListWidget();
}

void MyInspectorWidget::makeSegmentationActive(SegmentationDataWrapper sdw)
{
	currentDataSrc = DataWrapperContainer(sdw);
	populateSegmentElementsListWidget();
}

void MyInspectorWidget::makeSegmentationActive(const OmId segmentationID, const OmId segmentJustSelectedID)
{
	currentDataSrc = DataWrapperContainer(SEGMENTATION, segmentationID);
	populateSegmentElementsListWidget(true, segmentJustSelectedID);
}

void MyInspectorWidget::makeSegmentationActive(SegmentationDataWrapper sdw, const OmId segmentJustSelectedID)
{
	currentDataSrc = DataWrapperContainer(sdw);
	populateSegmentElementsListWidget(true, segmentJustSelectedID);
}

void MyInspectorWidget::addSegment()
{
	const OmId segmentationID = segInspectorWidget->getSegmentationID();
	OmSegment & added_segment = OmVolume::GetSegmentation(segmentationID).AddSegment();
	rebuildSegmentList(segmentationID, added_segment.GetId());
}

void MyInspectorWidget::refreshWidgetData()
{
	populateDataSrcListWidget();
	populateSegmentElementsListWidget();
}

void MyInspectorWidget::rebuildSegmentList(const OmId segmentationID)
{
	populateDataSrcListWidget();

	hashOfSementationsAndSegments.remove(segmentationID);
	makeSegmentationActive(segmentationID);
}

void MyInspectorWidget::rebuildSegmentList(const OmId segmentationID,
					   const OmId segmentJustAddedID)
{
	populateDataSrcListWidget();

	hashOfSementationsAndSegments.remove(segmentationID);
	makeSegmentationActive(segmentationID, segmentJustAddedID );
}

void MyInspectorWidget::populateSegmentElementsListWidget(const bool doScrollToSelectedSegment,
							  const OmId segmentJustSelectedID)
{
	debug("guievent", "in %s...\n", __FUNCTION__ );

	SegmentationDataWrapper sdw = currentDataSrc.getSegmentationDataWrapper();
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

	setTabEnabled( setupDataElementList(), "All Segments" );
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
		/*
		SegmentDataSet mapped;
		segmentation.MapValuesToSegmentId( seg.getID(), mapped );
		QString new_note("");
		foreach( SEGMENT_DATA_TYPE mapped_seg, mapped ){
			new_note += QString::number(mapped_seg) + ", ";
		}
		seg.setNote( new_note );
		*/
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

	autoResizeColumnWidths(dataElementsWidget, 3);

	if (doScrollToSelectedSegment && rowToJumpTo != NULL) {
		dataElementsWidget->scrollToItem(rowToJumpTo, QAbstractItemView::PositionAtCenter);
	}
}

void MyInspectorWidget::leftClickOnSegment(QTreeWidgetItem * current, const int column)
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

void MyInspectorWidget::sendSegmentChangeEvent(SegmentDataWrapper sdw, const bool augment_selection)
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
				   "myInspectorWidget"))->Run();
}

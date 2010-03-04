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


#include "system/omEventManager.h"
#include "system/events/omView3dEvent.h"
#include "system/events/omViewEvent.h"

#include <vmmlib/vmmlib.h>
#include <vmmlib/serialization.h>
#include "common/omDebug.h"
using namespace vmml;

Q_DECLARE_METATYPE(DataWrapperContainer);
Q_DECLARE_METATYPE(FilterDataWrapper);

MyInspectorWidget::MyInspectorWidget(QWidget * parent)
 : QWidget(parent)
{
	layoutWidget = new QWidget( this );
	QVBoxLayout *verticalLayout = new QVBoxLayout(layoutWidget);
	verticalLayout->addWidget(setupVolumeList(layoutWidget));
	verticalLayout->addWidget(setupDataSrcList());
	dataElementsTabs = new QTabWidget();
	verticalLayout->addWidget(dataElementsTabs);

	currentDataSrc = DataWrapperContainer();
	inspectorProperties = new InspectorProperties( this );
	segmentList = new SegmentList(this, inspectorProperties, dataElementsTabs  );

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

	GuiUtils::autoResizeColumnWidths(dataSrcListWidget, 3);

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

	GuiUtils::autoResizeColumnWidths(filterListWidget, 3);

	filterListWidget->update();
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
	if (!dataSrcItem) {	// right click occured in "white space" of widget (not on actual item, like channe1, etc)
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
	segmentList->makeSegmentationActive(segmentationID);
}

void MyInspectorWidget::makeSegmentationActive(SegmentationDataWrapper sdw)
{
	segmentList->makeSegmentationActive(sdw);
}

void MyInspectorWidget::makeSegmentationActive(const OmId segmentationID, const OmId segmentJustSelectedID)
{
	segmentList->makeSegmentationActive(segmentationID, segmentJustSelectedID);
}

void MyInspectorWidget::makeSegmentationActive(SegmentationDataWrapper sdw, const OmId segmentJustSelectedID)
{
	segmentList->makeSegmentationActive(sdw, segmentJustSelectedID);
}

void MyInspectorWidget::refreshWidgetData()
{
	populateDataSrcListWidget();
	segmentList->populateSegmentElementsListWidget();
}

void MyInspectorWidget::rebuildSegmentList(const OmId segmentationID)
{
	populateDataSrcListWidget();
	segmentList->rebuildSegmentList(segmentationID);
}

void MyInspectorWidget::rebuildSegmentList(const OmId segmentationID,
					   const OmId segmentJustAddedID)
{
	populateDataSrcListWidget();
	segmentList->rebuildSegmentList( segmentationID, segmentJustAddedID);
}

void MyInspectorWidget::populateSegmentElementsListWidget(const bool doScrollToSelectedSegment,
							  const OmId segmentJustSelectedID)
{
	segmentList->populateSegmentElementsListWidget( doScrollToSelectedSegment,
							segmentJustSelectedID);
}

void MyInspectorWidget::addSegment()
{
	const OmId segmentationID = segInspectorWidget->getSegmentationID();
	OmSegment & added_segment = OmVolume::GetSegmentation(segmentationID).AddSegment();
	segmentList->rebuildSegmentList(segmentationID, added_segment.GetId());
}

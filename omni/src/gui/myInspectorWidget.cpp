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
	verticalLayout = new QVBoxLayout(this);
	
	//	verticalLayout->addWidget(setupVolumeList(layoutWidget));
	verticalLayout->addWidget(setupDataSrcList());

	currentDataSrc = DataWrapperContainer();

	inspectorProperties = new InspectorProperties( this );
	elementListBox = new ElementListBox(this, verticalLayout);
	segmentList = new SegmentList(this, inspectorProperties, elementListBox  );

	QMetaObject::connectSlotsByName(this);
}

MyInspectorWidget::~MyInspectorWidget()
{
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
	elementListBox->setTabEnabled( QString("Channel %1").arg(cdw.getID()), 
				      setupFilterList(), 
				       "Filters" );
				      
	filterListWidget->clear();

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


void MyInspectorWidget::addSegmentationToSplitter(SegmentationDataWrapper sdw)
{
	segInspectorWidget = new SegInspector( sdw, this);
	
	connect(segInspectorWidget, SIGNAL(segmentationBuilt(OmId)), 
		segmentList, SLOT(rebuildSegmentList(OmId)));
	
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


void MyInspectorWidget::addChannelToVolume()
{
	OmChannel & added_channel = OmVolume::AddChannel();
	addToVolume(&added_channel, CHANNEL);
}

void MyInspectorWidget::addSegmentationToVolume()
{
	OmSegmentation & added_segmentation = OmVolume::AddSegmentation();
	addToVolume(&added_segmentation, SEGMENTATION);
	segmentList->makeSegmentationActive(added_segmentation.GetId());
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


void MyInspectorWidget::leftClickOnFilterItem(QTreeWidgetItem * current, const int column)
{
	if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
		addToSplitterDataSource(current );
	}
}

void MyInspectorWidget::leftClickOnDataSourceItem(QTreeWidgetItem * current, const int column)
{
	if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
		doShowDataSrcContextMenu( current );
	} else {
		addToSplitterDataSource(current );
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

// called from mainwindow after segmentation or channel added
// "refresh datasrc list"
void MyInspectorWidget::addToVolume(OmManageableObject * item, ObjectType item_type)
{
	populateDataSrcListWidget();
}

void MyInspectorWidget::addFilter()
{
	ChannelDataWrapper cdw = channelInspectorWidget->getChannelDataWrapper();
	OmVolume::GetChannel(cdw.getID()).AddFilter();
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

void MyInspectorWidget::addChildrenToSegmentation(OmId seg_id)
{
	//debug("FIXME", << "MyInspectorWidget::addChildrenToSegmentation: mesh was built; should we do something? (purcaro)\n";
}

void MyInspectorWidget::SegmentObjectModificationEvent(OmSegmentEvent * event)
{
	segmentList->dealWithSegmentObjectModificationEvent(event);
}

void MyInspectorWidget::refreshWidgetData()
{
	populateDataSrcListWidget();
	segmentList->populateSegmentElementsListWidget();
}

void MyInspectorWidget::addSegment()
{
	const OmId segmentationID = segInspectorWidget->getSegmentationID();
	OmSegment & added_segment = OmVolume::GetSegmentation(segmentationID).AddSegment();
	segmentList->rebuildSegmentList(segmentationID, added_segment.GetId());
}



//////////////////////////////
///////// Data Source Box Stuff
//////////////////////////////
QTreeWidget *MyInspectorWidget::setupDataSrcList()
{
	dataSrcListWidget = new QTreeWidget(this);
	dataSrcListWidget->setAlternatingRowColors(false);
	dataSrcListWidget->setColumnCount(3);

	dataSrcListWidget->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );

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

void MyInspectorWidget::showDataSrcContextMenu(const QPoint & menuPoint)
{
	QTreeWidgetItem *dataSrcItem = dataSrcListWidget->itemAt(menuPoint);
	if (!dataSrcItem) {	// right click occured in "white space" of widget
		connect(makeDataSrcContextMenu(dataSrcListWidget),
			SIGNAL(triggered(QAction *)), this, SLOT(doDataSrcContextMenuVolAdd(QAction *)));

		contextMenuDataSrc->exec(dataSrcListWidget->mapToGlobal(menuPoint));
		return;
	}
	doShowDataSrcContextMenu( dataSrcItem );
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
		break;
	case VOLUME:
	case SEGMENT:
	case NOTE:
	case FILTER:
		break;
	}
}

void MyInspectorWidget::showChannelContextMenu()
{
	connect(makeContextMenuBase(dataSrcListWidget), SIGNAL(triggered(QAction *)), 
		this, SLOT(selectChannelView(QAction *)));

	contextMenu->exec(QCursor::pos());
}

QMenu *MyInspectorWidget::makeContextMenuBase(QTreeWidget * parent)
{
	xyAct = new QAction(tr("&View XY"), parent);
	xyAct->setStatusTip(tr("Opens the XY view"));

	xzAct = new QAction(tr("&View XZ"), parent);
	xzAct->setStatusTip(tr("Opens the XZ view"));

	yzAct = new QAction(tr("&View YZ"), parent);
	yzAct->setStatusTip(tr("Opens the YZ view"));

	propAct = new QAction(tr("&Properties"), parent);
	propAct->setStatusTip(tr("Opens properties"));

	contextMenu = new QMenu(parent);
	contextMenu->addAction(xyAct);
	contextMenu->addAction(xzAct);
	contextMenu->addAction(yzAct);
	contextMenu->addAction(propAct);

	return contextMenu;
}

void MyInspectorWidget::selectChannelView(QAction * act)
{
	ChannelDataWrapper cdw = getCurrentlySelectedChannel();
	if( propAct == act ){
		addChannelToSplitter(cdw);
	} else {
		emit triggerChannelView( cdw.getID(), getViewType(act));
	}
}

void MyInspectorWidget::addToSplitterDataSource(QTreeWidgetItem * current)
{
	QVariant result = current->data(USER_DATA_COL, Qt::UserRole);
	DataWrapperContainer dwc = result.value < DataWrapperContainer > ();

	switch (dwc.getType()) {
	case CHANNEL:
		populateFilterListWidget(dwc.getChannelDataWrapper());
		break;
	case SEGMENTATION:
		segmentList->makeSegmentationActive(dwc.getSegmentationDataWrapper());
		break;
	case VOLUME:
	case SEGMENT:
	case NOTE:
	case FILTER:
		break;
	}
}

ChannelDataWrapper MyInspectorWidget::getCurrentlySelectedChannel()
{
	QTreeWidgetItem *dataSrcItem = dataSrcListWidget->currentItem();
	QVariant result = dataSrcItem->data(USER_DATA_COL, Qt::UserRole);
	DataWrapperContainer dwc = result.value < DataWrapperContainer > ();
	return dwc.getChannelDataWrapper();
}

void MyInspectorWidget::showSegmentationContextMenu()
{
	connect(makeContextMenuBase(dataSrcListWidget), SIGNAL(triggered(QAction *)), 
		this, SLOT(selectSegmentationView(QAction *)));

	contextMenu->exec(QCursor::pos());
}

SegmentationDataWrapper MyInspectorWidget::getCurrentlySelectedSegmentation()
{
	QTreeWidgetItem *dataSrcItem = dataSrcListWidget->currentItem();
	QVariant result = dataSrcItem->data(USER_DATA_COL, Qt::UserRole);
	DataWrapperContainer dwc = result.value < DataWrapperContainer > ();
	return dwc.getSegmentationDataWrapper();
}

void MyInspectorWidget::selectSegmentationView(QAction * act)
{
	SegmentationDataWrapper sdw = getCurrentlySelectedSegmentation();
	segmentList->makeSegmentationActive(sdw);

	if( propAct == act ){
		addSegmentationToSplitter(sdw);
	} else {
		emit triggerSegmentationView( sdw.getID(), getViewType(act));
	}
}

void MyInspectorWidget::addChannelToSplitter(ChannelDataWrapper cdw)
{
	channelInspectorWidget = new ChanInspector( cdw, this);
	
	connect(channelInspectorWidget->addFilterButton, SIGNAL(clicked()), 
		this, SLOT(addFilter()));
	
	connect(channelInspectorWidget->nameEdit, SIGNAL(editingFinished()),
		this, SLOT(nameEditChanged()), Qt::DirectConnection);

	inspectorProperties->setOrReplaceWidget( channelInspectorWidget, 
						 QString("Channel %1 Inspector").arg(cdw.getID()) );
}

#include "utility/dataWrappers.h"
#include "gui/mstViewer.hpp"
#include "common/omDebug.h"
#include "gui/elementListBox.h"
#include "gui/guiUtils.h"
#include "gui/mainwindow.h"
#include "gui/myInspectorWidget.h"
#include "gui/segmentListWorking.h"
#include "gui/segmentListValid.h"
#include "gui/segmentListRecent.h"
#include "inspectors/chanInspector.h"
#include "inspectors/filObjectInspector.h"
#include "inspectors/inspectorProperties.h"
#include "inspectors/segInspector.h"
#include "project/omProject.h"
#include "segment/omSegment.h"
#include "segment/omSegmentCache.h"
#include "system/events/omView3dEvent.h"
#include "system/events/omViewEvent.h"
#include "system/omEventManager.h"
#include "system/omStateManager.h"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"
#include "system/viewGroup/omViewGroupState.h"

#include <QtGui>
#include <QMessageBox>

Q_DECLARE_METATYPE(DataWrapperContainer);
Q_DECLARE_METATYPE(FilterDataWrapper);

MyInspectorWidget::MyInspectorWidget(MainWindow * parent, OmViewGroupState * vgs)
	: QWidget(parent), mViewGroupState(vgs)
{

	mParentWindow = parent;

	verticalLayout = new QVBoxLayout(this);

	verticalLayout->addWidget(setupDataSrcList());

	currentDataSrc = boost::make_shared<DataWrapperContainer>();

	inspectorProperties = new InspectorProperties( this, mViewGroupState);

	elementListBox = new ElementListBox(this);
	verticalLayout->addWidget(elementListBox);

	segmentList = new SegmentListWorking(this, inspectorProperties, elementListBox);
	validList = new SegmentListValid(this, inspectorProperties, elementListBox);
	recentList = new SegmentListRecent(this, inspectorProperties, elementListBox);
	channelInspectorWidget=NULL;
	segInspectorWidget=NULL;

	QMetaObject::connectSlotsByName(this);

	OmStateManager::setInspector( this );
}

MyInspectorWidget::~MyInspectorWidget()
{
	OmStateManager::setInspector( NULL );
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

void MyInspectorWidget::setRowFlagsAndCheckState(QTreeWidgetItem * row, Qt::CheckState checkState)
{
	row->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
	row->setCheckState(ENABLED_COL, checkState);
}

void MyInspectorWidget::populateDataSrcListWidget()
{
	dataSrcListWidget->clear();

	foreach(OmId channID, OmProject::GetValidChannelIds()) {
		DataWrapperContainer dwc = DataWrapperContainer(CHANNEL, channID);
		ChannelDataWrapper cdw = dwc.getChannelDataWrapper();
		QTreeWidgetItem *row = new QTreeWidgetItem(dataSrcListWidget);
		row->setText(NAME_COL, cdw.getName());
		row->setText(ID_COL, QString("%1").arg(cdw.getID()));
		row->setText(NOTE_COL, cdw.getNote());
		row->setData(USER_DATA_COL, Qt::UserRole, qVariantFromValue(dwc));
		setRowFlagsAndCheckState(row, GuiUtils::getCheckState(cdw.isEnabled()));
	}

	foreach(OmId segmenID, OmProject::GetValidSegmentationIds()) {
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
		this, SLOT(leftClickOnDataSourceItem(QTreeWidgetItem *)));

	GuiUtils::autoResizeColumnWidths(dataSrcListWidget, 3);

	dataSrcListWidget->update();
}

void MyInspectorWidget::populateFilterListWidget(ChannelDataWrapper cdw)
{
	elementListBox->reset();
	elementListBox->setTitle(QString("Channel %1").arg(cdw.getID()));
	elementListBox->addTab(0, setupFilterList(), "Filters" );

	filterListWidget->clear();

	filterListWidget->selectionModel()->blockSignals(true);
	filterListWidget->selectionModel()->clearSelection();

	foreach( FilterDataWrapper filter, cdw.getAllFilterIDsAndNames()) {
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
                this, SLOT(addToSplitterDataElementFilter(QTreeWidgetItem *)));

	GuiUtils::autoResizeColumnWidths(filterListWidget, 3);

	filterListWidget->update();
}

void MyInspectorWidget::addSegmentationToSplitter(SegmentationDataWrapper sdw)
{
	segInspectorWidget = new SegInspector( sdw, this);

	connect(segInspectorWidget->nameEdit, SIGNAL(editingFinished()),
		this, SLOT(nameEditChanged()), Qt::DirectConnection);

	connect(segInspectorWidget->nameEdit, SIGNAL(editingFinished()),
		this, SLOT(nameEditChanged()), Qt::DirectConnection);

	inspectorProperties->setOrReplaceWidget( segInspectorWidget,
						 QString("Segmentation %1 Inspector").arg(sdw.getID()) );
}

void MyInspectorWidget::addToSplitterDataElementFilter(QTreeWidgetItem * current)
{
	QVariant result = current->data(USER_DATA_COL, Qt::UserRole);
	FilterDataWrapper fdw = result.value < FilterDataWrapper > ();

	filObjectInspectorWidget = new FilObjectInspector(this, fdw);

	inspectorProperties->setOrReplaceWidget( filObjectInspectorWidget,
						 QString("Filter %1 Inspector").arg(fdw.getID()) );
}

void MyInspectorWidget::addChannelToVolume()
{
	OmChannel & added_channel = OmProject::AddChannel();
	populateDataSrcListWidget();

	ChannelDataWrapper cdw( added_channel.GetId() );
	addChannelToSplitter(cdw);
}

QString MyInspectorWidget::getSegmentationGroupBoxTitle(SegmentationDataWrapper sdw)
{
	return QString("Segmentation %1: Segments").arg(sdw.getID());
}

void MyInspectorWidget::addSegmentationToVolume()
{
	OmSegmentation & added_segmentation = OmProject::AddSegmentation();
	populateDataSrcListWidget();

	SegmentationDataWrapper sdw( added_segmentation.GetId() );
	updateSegmentListBox( sdw );

	addSegmentationToSplitter( sdw);
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

void MyInspectorWidget::leftClickOnFilterItem(QTreeWidgetItem * current)
{
	if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
		addToSplitterDataSource(current );
	}
}

void MyInspectorWidget::leftClickOnDataSourceItem(QTreeWidgetItem * current)
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

void MyInspectorWidget::addFilter()
{
	ChannelDataWrapper cdw = channelInspectorWidget->getChannelDataWrapper();
	OmProject::GetChannel(cdw.getID()).AddFilter();
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

void MyInspectorWidget::refreshWidgetData()
{
	populateDataSrcListWidget();
	segmentList->populateSegmentElementsListWidget();
	validList->populateSegmentElementsListWidget();
	recentList->populateSegmentElementsListWidget();
}

void MyInspectorWidget::rebuildSegmentLists(const OmId segmentationID, const OmSegID segID)
{
	segmentList->rebuildSegmentList(segmentationID, segID);
	validList->rebuildSegmentList(segmentationID, segID);
	recentList->rebuildSegmentList(segmentationID, segID);
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
	}
}

void MyInspectorWidget::showChannelContextMenu()
{
	connect(makeContextMenuBase(dataSrcListWidget), SIGNAL(triggered(QAction *)),
		this, SLOT(selectChannelView(QAction *)));

	contextMenu->exec(QCursor::pos());
}

void MyInspectorWidget::showSegmentationContextMenu()
{
	connect(makeSegmentationContextMenu(dataSrcListWidget), SIGNAL(triggered(QAction *)),
		this, SLOT(selectSegmentationView(QAction *)));

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

	delAct = new QAction(tr("&Delete"), parent);
        propAct->setStatusTip(tr("Deletes a Volume"));

	contextMenu = new QMenu(parent);
	contextMenu->addAction(xyAct);
	contextMenu->addAction(xzAct);
	contextMenu->addAction(yzAct);
	contextMenu->addAction(propAct);
	contextMenu->addAction(delAct);

	return contextMenu;
}

QMenu* MyInspectorWidget::makeSegmentationContextMenu(QTreeWidget* parent)
{
	examMSTAct = new QAction(tr("&Examine MST"), parent);
	examMSTAct->setStatusTip(tr("View the MST table"));

	QMenu* menu = makeContextMenuBase(parent);
	menu->addAction(examMSTAct);

	return menu;
}

void MyInspectorWidget::selectChannelView(QAction * act)
{
	ChannelDataWrapper cdw = getCurrentlySelectedChannel();
	if( propAct == act ){
		addChannelToSplitter(cdw);
	} else if (delAct == act) {
		deleteChannel(cdw);
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
		updateSegmentListBox( dwc.getSegmentationDataWrapper() );
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
	updateSegmentListBox( sdw );

	if( propAct == act ){
		addSegmentationToSplitter(sdw);
	} else if (delAct == act) {
		deleteSegmentation(sdw);
	} else if (examMSTAct == act ){
		showMSTtable(sdw);
	} else {
		emit triggerSegmentationView( sdw.getID(), getViewType(act));
	}
}

void MyInspectorWidget::addChannelToSplitter(ChannelDataWrapper cdw)
{
	channelInspectorWidget = new ChanInspector( cdw, this);

	connect(channelInspectorWidget->addFilterButton, SIGNAL(clicked()),
		this, SLOT(addFilter()));

	inspectorProperties->setOrReplaceWidget( channelInspectorWidget,
						 QString("Channel %1 Inspector").arg(cdw.getID()) );
}

void MyInspectorWidget::deleteSegmentation(SegmentationDataWrapper sdw)
{
        QMessageBox msgBox;
        msgBox.setText("Volume is about to be deleted!!");
        msgBox.setInformativeText("Are you certain you want to delete this volume?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();

        if (ret == QMessageBox::Yes){

		elementListBox->reset();

		mParentWindow->cleanViewsOnVolumeChange(CHANNEL, sdw.getID());
		foreach(OmId channelID, OmProject::GetValidChannelIds()) {
			OmChannel & channel = OmProject::GetChannel(channelID);
			foreach(OmId filterID, channel.GetValidFilterIds()) {
				OmFilter2d &filter = channel.GetFilter(filterID);
				if (filter.GetSegmentation() == sdw.getID()){
					mParentWindow->cleanViewsOnVolumeChange(CHANNEL,channelID);
				}
			}
		}
		mParentWindow->cleanViewsOnVolumeChange(SEGMENTATION, sdw.getID());

		inspectorProperties->closeDialog();

		OmProject::RemoveSegmentation(sdw.getID());
		populateDataSrcListWidget();
        }
}

void MyInspectorWidget::deleteChannel(ChannelDataWrapper cdw)
{
        QMessageBox msgBox;
        msgBox.setText("Volume is about to be deleted!!");
        msgBox.setInformativeText("Are you certain you want to delete this volume?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();

        if (ret == QMessageBox::Yes){
            inspectorProperties->closeDialog();
            elementListBox->reset();
            mParentWindow->cleanViewsOnVolumeChange(CHANNEL, cdw.getID());
            OmProject::RemoveChannel(cdw.getID());
            populateDataSrcListWidget();
        }
}

void MyInspectorWidget::updateSegmentListBox( SegmentationDataWrapper sdw )
{
	elementListBox->reset();
	elementListBox->setTitle(getSegmentationGroupBoxTitle(sdw));
	segmentList->makeSegmentationActive( sdw, NULL_SEGMENT_VALUE, true );
	validList->makeSegmentationActive( sdw, NULL_SEGMENT_VALUE, true );
	recentList->makeSegmentationActive( sdw, NULL_SEGMENT_VALUE, true );
}

void MyInspectorWidget::SegmentObjectModificationEvent(OmSegmentEvent * event)
{
	elementListBox->reset();
	elementListBox->setTitle("");

	const OmId segmentationID1 = segmentList->dealWithSegmentObjectModificationEvent(event);
	const OmId segmentationID2 = validList->dealWithSegmentObjectModificationEvent(event);
	recentList->dealWithSegmentObjectModificationEvent(event);

	if( segmentationID1 > 0 && segmentationID1 == segmentationID2 ){
		SegmentationDataWrapper sdw(segmentationID1);
		elementListBox->setTitle(getSegmentationGroupBoxTitle(sdw));
	}
}

void MyInspectorWidget::showMSTtable(SegmentationDataWrapper sdw)
{
	new MstViewer(this, sdw);
}

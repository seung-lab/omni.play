
#include <QtGui> 
#include "myInspectorWidget.h" 

#include"ui_segInspector.h"
#include"segInspector.h"

#include "ui_volumeInspector.h"
#include "volumeInspector.h"

#include "ui_chanInspector.h"
#include "chanInspector.h"

#include "ui_segObjectInspector.h"
#include "segObjectInspector.h"

#include "ui_filObjectInspector.h"
#include "filObjectInspector.h"

#include "ui_preferences3d.h"
#include "preferences3d.h"

#include "ui_preferences2d.h"
#include "preferences2d.h"

#include "ui_preferencesMesh.h"
#include "preferencesMesh.h"

#include "volume/omVolumeTypes.h"
#include "common/omStd.h"
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
using namespace vmml;

Q_DECLARE_METATYPE( DataWrapperFactory );
Q_DECLARE_METATYPE( SegmentDataWrapper );
Q_DECLARE_METATYPE( FilterDataWrapper );

#define DEBUG 0

MyInspectorWidget::MyInspectorWidget(QWidget *parent) : QWidget(parent)
{ 
	segmenHelper  = new SegmentationHelper(this);
	channelHelper = new ChannelHelper(this);

	resize(448, 640);
	QSizePolicy mSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	mSizePolicy.setHorizontalStretch(0);
	mSizePolicy.setVerticalStretch(0);
	mSizePolicy.setHeightForWidth(sizePolicy().hasHeightForWidth());
	setSizePolicy(mSizePolicy);

	splitter = new QSplitter(this);
	splitter->setOrientation(Qt::Horizontal);
	QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Expanding);
	sizePolicy1.setHorizontalStretch(0);
	sizePolicy1.setVerticalStretch(0);
	sizePolicy1.setHeightForWidth(splitter->sizePolicy().hasHeightForWidth());
	splitter->setSizePolicy(sizePolicy1);

	QWidget *layoutWidget = new QWidget(splitter);
	splitter->addWidget( layoutWidget );
	QVBoxLayout *verticalLayout = new QVBoxLayout( layoutWidget );
	verticalLayout->setContentsMargins(0, 0, 0, 0);

	// Volumes/Preferences (2D view, Mesh, etc..)
	verticalLayout->addWidget( setupVolumeList( layoutWidget ) );

	// data source list (Channel, Segmentation, etc)
	verticalLayout->addWidget( setupDataSrcList() );

	// data elements: segments, filters, etc (and search widgets)
	//	verticalLayout->addWidget( setupDataElementList() );
	dataElementsTabs = new QTabWidget();
	verticalLayout->addWidget( dataElementsTabs );
	dataElementsTabs->addTab( setupDataElementList(), "All" );

	//	verticalLayout->addLayout( setupSearchStuff() );

	// setup main grid layout
	QVBoxLayout *verticalLayout_2 = new QVBoxLayout();
	verticalLayout_2->addWidget(splitter);
	QGridLayout *gridLayout       = new QGridLayout(this);
	gridLayout->addLayout(verticalLayout_2, 0, 0, 1, 1);


	current_object = 99;
	first_access = true;
	iSentIt = false;
	preferencesActivated = false;

	// what does this do? (purcaro)
	QMetaObject::connectSlotsByName(this);
}

QTreeWidget* MyInspectorWidget::setupDataElementList()
{
	dataElementsWidget = new QTreeWidget(this);
	dataElementsWidget->setSelectionMode( QAbstractItemView::ExtendedSelection );
	dataElementsWidget->setAlternatingRowColors( true );

     dataElementsWidget->setColumnCount(3);
     QStringList headers;
     headers << tr("enabled") << tr("Name") << tr("ID") << tr("Notes");
     dataElementsWidget->setHeaderLabels(headers);

	return dataElementsWidget;
}

QTreeWidget* MyInspectorWidget::setupDataSrcList()
{
	dataSrcListWidget = new QTreeWidget(this);
	dataSrcListWidget->setAlternatingRowColors( true );
     dataSrcListWidget->setColumnCount(3);
     QStringList headers;
     headers << tr("enabled") << tr("Name") << tr("ID") << tr("Notes");
     dataSrcListWidget->setHeaderLabels(headers);

	QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Minimum);
	sizePolicy3.setHorizontalStretch(0);
	sizePolicy3.setVerticalStretch(0);
	sizePolicy3.setHeightForWidth( dataSrcListWidget->sizePolicy().hasHeightForWidth());
	dataSrcListWidget->setSizePolicy( sizePolicy3 );

	populateDataSrcListWidget();
	
	// left click to open inspectors
	connect( dataSrcListWidget, SIGNAL( itemClicked( QTreeWidgetItem *, int )),
		    this, SLOT( addToSplitterDataSource(QTreeWidgetItem *, int )));

	// context menu setup
	dataSrcListWidget->setContextMenuPolicy( Qt::CustomContextMenu );
	connect( dataSrcListWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
		this, SLOT(showDataSrcContextMenu(const QPoint&)));

	return dataSrcListWidget;
}

QTreeWidget* MyInspectorWidget::setupVolumeList( QWidget *layoutWidget )
{
	QTreeWidget *volumeListWidget = new QTreeWidget( layoutWidget );
	volumeListWidget->setMaximumSize(QSize(16777215, 75));
	volumeListWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	volumeListWidget->setHeaderHidden( true );

	QTreeWidgetItem* volItem = new QTreeWidgetItem(volumeListWidget);
	volItem->setText(0, "Volume" );
	QTreeWidgetItem* view2dItem = new QTreeWidgetItem(volumeListWidget);
	view2dItem->setText(0, "2D View" );
	QTreeWidgetItem* view3dItem = new QTreeWidgetItem(volumeListWidget);
	view3dItem->setText(0, "3D View" );
	QTreeWidgetItem* meshItem = new QTreeWidgetItem(volumeListWidget);
	meshItem->setText(0, "Mesh");

	QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Maximum);
	sizePolicy2.setHorizontalStretch(0);
	sizePolicy2.setVerticalStretch(0);
	sizePolicy2.setHeightForWidth(volumeListWidget->sizePolicy().hasHeightForWidth());
	volumeListWidget->setSizePolicy(sizePolicy2);

	connect(volumeListWidget, SIGNAL( itemClicked( QTreeWidgetItem *, int )  ),
		   this, SLOT(addPreferencesToSplitter( QTreeWidgetItem *, int)));

	return volumeListWidget;
}

MyInspectorWidget::~MyInspectorWidget()
{
	delete(splitter);
}

Qt::CheckState MyInspectorWidget::getCheckState( const bool enabled )
{
	if( enabled ){
		return Qt::Checked;
	} else {
		return Qt::Unchecked;
	}
}

void MyInspectorWidget::setRowFlagsAndCheckState( QTreeWidgetItem *row, Qt::CheckState checkState )
{
	row->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled );
	row->setCheckState( ENABLED_COL, checkState );
}

void MyInspectorWidget::populateDataSrcListWidget()
{
	dataSrcListWidget->clear();
	
	foreach( OmId channID, OmVolume::GetValidChannelIds() ){
		DataWrapperFactory dwf = DataWrapperFactory( CHANNEL, channID );
		ChannelDataWrapper cdw = dwf.getChannelDataWrapper();
		QTreeWidgetItem *row = new QTreeWidgetItem( dataSrcListWidget );
		row->setText( NAME_COL, cdw.getName() );
		row->setText( ID_COL, QString( "%1").arg(cdw.getID() ));
		row->setText( NOTE_COL, cdw.getNote() );
		row->setData( USER_DATA_COL, Qt::UserRole, qVariantFromValue( dwf ) );
		setRowFlagsAndCheckState( row, getCheckState( cdw.isEnabled() ) );
	}

	foreach( OmId segmenID, OmVolume::GetValidSegmentationIds() ){
		DataWrapperFactory dwf = DataWrapperFactory( SEGMENTATION, segmenID );
		SegmentationDataWrapper sdw = dwf.getSegmentationDataWrapper();
		QTreeWidgetItem *row = new QTreeWidgetItem( dataSrcListWidget );
		row->setText( NAME_COL, sdw.getName() );
		row->setText( ID_COL, QString( "%1").arg(sdw.getID() ));
		row->setText( NOTE_COL, sdw.getNote() );
		row->setData( USER_DATA_COL, Qt::UserRole, qVariantFromValue( dwf ) );
		setRowFlagsAndCheckState( row, getCheckState( sdw.isEnabled() ) );
	}

	for( int i = 0; i <= MAX_COL_TO_DISPLAY; i++ ){
		dataSrcListWidget->resizeColumnToContents(i);
	}

	dataSrcListWidget->update();
}

void MyInspectorWidget::rebuildSegmentList( const OmId segmentationID )
{
	hashOfSementationsAndSegments.remove( segmentationID );
	populateDataSrcListWidget();
	
	if( isThereASegmentationSelected() ){
		SegmentationDataWrapper sdw = getCurrentlySelectedSegmentation();
		populateSegmentElementsListWidget( sdw );	
	}
}

void MyInspectorWidget::populateSegmentElementsListWidget( SegmentationDataWrapper sdw )
{
	dataElementsWidget->clear();

	const OmId segmenID = sdw.getID();

	if( !hashOfSementationsAndSegments.contains( segmenID ) ){
		hashOfSementationsAndSegments[ segmenID ] = sdw.getAllSegmentIDsAndNames();
	} 
	QHash<OmId, SegmentDataWrapper> segs = hashOfSementationsAndSegments[ segmenID ];

	dataElementsWidget->selectionModel()->blockSignals(true);
	dataElementsWidget->selectionModel()->clearSelection();

	foreach( SegmentDataWrapper seg, segs ){
		QTreeWidgetItem *row = new QTreeWidgetItem( dataElementsWidget );
		row->setText( NAME_COL, seg.getName() );
		row->setText( ID_COL, QString("%1").arg(seg.getID() ));
		row->setData( USER_DATA_COL, Qt::UserRole, qVariantFromValue( seg ) );
		row->setText( NOTE_COL, seg.getNote() );
		setRowFlagsAndCheckState( row, getCheckState( seg.isCheckedOff() ) );
		row->setSelected( seg.isSelected() );
	}
	
	dataElementsWidget->selectionModel()->blockSignals(false);
		
	dataElementsWidget->disconnect( SIGNAL( itemClicked( QTreeWidgetItem *, int )) );
	connect( dataElementsWidget, SIGNAL( itemClicked( QTreeWidgetItem *, int )),
		    this, SLOT( addToSplitterDataElementSegment(QTreeWidgetItem *, int )));

	for( int i = 0; i < MAX_COL_TO_DISPLAY; i++ ){
		dataElementsWidget->resizeColumnToContents(i);
	}

	dataElementsWidget->update();
}

void MyInspectorWidget::populateChannelElementsListWidget( ChannelDataWrapper cdw )
{
	dataElementsWidget->clear();
	
	const OmId segmenID = cdw.getID();

	dataElementsWidget->selectionModel()->blockSignals(true);
	dataElementsWidget->selectionModel()->clearSelection();

	foreach( FilterDataWrapper filter, cdw.getAllFilterIDsAndNames() ){
		QTreeWidgetItem *row = new QTreeWidgetItem( dataElementsWidget );
		row->setText( NAME_COL, filter.getName() );
		row->setText( ID_COL, QString("%1").arg(filter.getID() ));
		row->setData( USER_DATA_COL, Qt::UserRole, qVariantFromValue( filter ) );
		//row->setText( NOTE_COL, filter.getNote() );
		setRowFlagsAndCheckState( row, getCheckState( true ) );
		//row->setSelected( seg.isSelected() );
	}
	
	dataElementsWidget->selectionModel()->blockSignals(false);
		
	dataElementsWidget->disconnect( SIGNAL( itemClicked( QTreeWidgetItem *, int )) );
	connect( dataElementsWidget, SIGNAL( itemClicked( QTreeWidgetItem *, int )),
		    this, SLOT( addToSplitterDataElementFilter(QTreeWidgetItem *, int )));

	dataElementsWidget->update();
	for( int i = 0; i < MAX_COL_TO_DISPLAY; i++ ){
		dataElementsWidget->resizeColumnToContents(i);
	}
}

void MyInspectorWidget::addPreferencesToSplitter(QTreeWidgetItem *item, const int column )
{
	DOUT("addToSplitterPreferences");
	
	QWidget* my_widget = splitter->widget(1);
	QString text = item->text(0);

	QList<int> my_sizes = splitter->sizes();
	delete my_widget;
	
	if (text == "Volume") {
		volumeInspectorWidget = new VolumeInspector(splitter);
		connect(volumeInspectorWidget->addChannelButton, SIGNAL(clicked()), this, SIGNAL(addChannel()));
		connect(volumeInspectorWidget->addSegmentationButton, SIGNAL(clicked()), this, SIGNAL(addSegmentation()));
	} else if (text == "2D View") {		
		preferences2dWidget = new Preferences2d(splitter);
	} else if (text == "3D View") {		
		preferences3dWidget = new Preferences3d(splitter);
	} else if (text == "Mesh") {		
		preferencesMeshWidget = new PreferencesMesh(splitter);
	}
	
	if(!(first_access)) {
		splitter->setSizes(my_sizes);
	}
	
	first_access = false;
	preferencesActivated = true;
}

void MyInspectorWidget::addChannelToSplitter( ChannelDataWrapper data )
{
	const OmId item_id = data.getID();

	QWidget* my_widget = splitter->widget(1);

	if((current_object != CHANNEL) || (preferencesActivated)) {
		
		QList<int> my_sizes = splitter->sizes();
		delete my_widget;	
		
		channelInspectorWidget = new ChanInspector( item_id, splitter);

		connect( channelInspectorWidget->addFilterButton, SIGNAL(clicked()), 
			    this, SLOT(addFilter( )));
		
		if(!(first_access)) {
			splitter->setSizes(my_sizes);
		}
	}

	channelInspectorWidget->setId(item_id);
	populateChannelInspector(item_id);
	channelInspectorWidget->setChannelID( item_id );

	connect( channelInspectorWidget->nameEdit, SIGNAL(editingFinished()), 
		    this, SLOT(nameEditChanged()), Qt::DirectConnection);
	current_object = CHANNEL;
}

void MyInspectorWidget::addSegmentationToSplitter( SegmentationDataWrapper data )
{
	const OmId item_id = data.getID();

	QWidget* my_widget = splitter->widget(1);

	if((current_object != SEGMENTATION) || (preferencesActivated)) {
		
		QList<int> my_sizes = splitter->sizes();
		delete my_widget;	
		
		segInspectorWidget = new SegInspector(item_id, splitter);
		
		//		connect( segInspectorWidget, SIGNAL(meshBuilt(OmId)), 
		//			    this, SLOT(addChildrenToSegmentation(OmId)));

		connect( segInspectorWidget, SIGNAL(segmentationBuilt(OmId)), 
			    this, SLOT(rebuildSegmentList(OmId)));
		
		connect( segInspectorWidget->addSegmentButton, SIGNAL(clicked()), 
			    this, SLOT(addSegment()));
		
		if(!(first_access))
			splitter->setSizes(my_sizes);
	}

	segInspectorWidget->setId(item_id);
	populateSegmentationInspector(item_id);
	segInspectorWidget->setSegmentationID( item_id );
			
	connect( segInspectorWidget->nameEdit, SIGNAL(editingFinished()), 
		    this, SLOT(nameEditChanged()), Qt::DirectConnection);
	current_object = SEGMENTATION;
}

void MyInspectorWidget::addToSplitterDataElementSegment( QTreeWidgetItem * current, const int column )
{
	QVariant result  = current->data( USER_DATA_COL, Qt::UserRole );
	SegmentDataWrapper sdw = result.value< SegmentDataWrapper >(); 

	const OmId item_id = sdw.getID();
	QWidget* my_widget = splitter->widget(1);

	if((current_object != SEGMENT) || (preferencesActivated)) {
		QList<int> my_sizes = splitter->sizes();
		delete my_widget;
		segObjectInspectorWidget = new SegObjectInspector(splitter);
		segObjectInspectorWidget->nameEdit_2->setReadOnly(true);
		connect( segObjectInspectorWidget->colorButton, SIGNAL(clicked()), 
			    this, SLOT(setSegObjColor()));
		if(!(first_access))
			splitter->setSizes(my_sizes);
	}

	populateSegmentObjectInspector( sdw.getSegmentationID(), item_id);
			
	segObjectInspectorWidget->setSegmentationID( sdw.getSegmentationID() );
	segObjectInspectorWidget->setSegmentID( item_id );

	connect( segObjectInspectorWidget->nameEdit, SIGNAL(editingFinished()), 
		    this, SLOT(nameEditChanged()), Qt::DirectConnection);
	current_object = SEGMENT;

	first_access = false;
	preferencesActivated = false;
}

void MyInspectorWidget::addToSplitterDataElementFilter( QTreeWidgetItem * current, const int column )
{
	QVariant result  = current->data( USER_DATA_COL, Qt::UserRole );
	FilterDataWrapper fdw = result.value< FilterDataWrapper >(); 

	const OmId item_id = fdw.getID();
	QWidget* my_widget = splitter->widget(1);

	if((current_object != FILTER) || (preferencesActivated)) {
		QList<int> my_sizes = splitter->sizes();
		delete my_widget;
		filObjectInspectorWidget = new FilObjectInspector(splitter);
		connect( filObjectInspectorWidget->alphaSlider, SIGNAL(valueChanged(int)), 
			    this, SLOT(setFilAlpha(int)));
		if(!(first_access))
			splitter->setSizes(my_sizes);
	}

	populateFilterObjectInspector( fdw.getChannelID(), item_id);

	filObjectInspectorWidget->setChannelID(  fdw.getChannelID() );
	filObjectInspectorWidget->setFilterID( item_id );

	connect( filObjectInspectorWidget->nameEdit, SIGNAL(editingFinished()), 
		    this, SLOT(sourceEditChangedChan()), Qt::DirectConnection);
	connect( filObjectInspectorWidget->nameEdit_2, SIGNAL(editingFinished()), 
		    this, SLOT(sourceEditChangedSeg()), Qt::DirectConnection);
	current_object = FILTER;

	first_access = false;
	preferencesActivated = false;
}

void MyInspectorWidget::addToSplitterDataSource( QTreeWidgetItem * current, const int column )
{
	QVariant result  = current->data( USER_DATA_COL, Qt::UserRole );
	DataWrapperFactory dwf = result.value< DataWrapperFactory >(); 

	switch( dwf.getType()){
	case CHANNEL:
		addChannelToSplitter( dwf.getChannelDataWrapper() );
		populateChannelElementsListWidget( dwf.getChannelDataWrapper() );
		break;
	case SEGMENTATION:
		addSegmentationToSplitter( dwf.getSegmentationDataWrapper() );
		populateSegmentElementsListWidget( dwf.getSegmentationDataWrapper() );
		break;
	}

	first_access = false;
	preferencesActivated = false;
}

void MyInspectorWidget::showChannelContextMenu( const QPoint& menuPoint )
{
	connect( makeContextMenuBase( menuPoint, dataSrcListWidget ), 
		    SIGNAL(triggered(QAction *)), 
		    this, 
		    SLOT(selectChannelView(QAction *)));

	contextMenu->exec( dataSrcListWidget->mapToGlobal(menuPoint) );
}

void MyInspectorWidget::showSegmentationContextMenu( const QPoint& menuPoint )
{
	connect( makeContextMenuBase( menuPoint, dataSrcListWidget ), 
		    SIGNAL(triggered(QAction *)), 
		    this, 
		    SLOT(selectSegmentationView(QAction *)));

	contextMenu->exec( dataSrcListWidget->mapToGlobal(menuPoint) );
}

QMenu* MyInspectorWidget::makeDataSrcContextMenu( QTreeWidget* parent  )
{
	addChannelAct = new QAction(tr("Add Channel"), parent );
			
	addSegmentationAct = new QAction(tr("Add Segmentation"), parent );
			
	contextMenuDataSrc = new QMenu( parent );
	contextMenuDataSrc->addAction( addChannelAct );
	contextMenuDataSrc->addAction( addSegmentationAct );

	return contextMenuDataSrc;
}

QMenu* MyInspectorWidget::makeContextMenuBase( const QPoint& menuPoint, QTreeWidget* parent  )
{
	xyAct = new QAction(tr("&View XY"), parent );
	xyAct->setStatusTip(tr("Opens the XY view"));
			
	xzAct = new QAction(tr("&View XZ"), parent );
	xzAct->setStatusTip(tr("Opens the XZ view"));
			
	yzAct = new QAction(tr("&View YZ"), parent );
	yzAct->setStatusTip(tr("Opens the YZ view"));
			
	contextMenu = new QMenu( parent );
	contextMenu->addAction(xyAct);
	contextMenu->addAction(xzAct);
	contextMenu->addAction(yzAct);

	return contextMenu;
}


void  MyInspectorWidget::addChannelToVolume()
{
	OmChannel& added_channel = OmVolume::AddChannel();
	addToVolume(&added_channel, CHANNEL);
}

void  MyInspectorWidget::addSegmentationToVolume()
{
	OmSegmentation& added_segmentation = OmVolume::AddSegmentation();
	addToVolume(&added_segmentation, SEGMENTATION);
}

void MyInspectorWidget::doDataSrcContextMenuVolAdd( QAction *act )
{
  	if(act == addChannelAct ) {
		addChannelToVolume();
	} else if(act == addSegmentationAct ) {
		addSegmentationToVolume();
	} else {
		throw OmFormatException("could not match QAction type...\n");
	}
}


void MyInspectorWidget::showDataSrcContextMenu( const QPoint& menuPoint )
{
	QTreeWidgetItem *dataSrcItem = dataSrcListWidget->itemAt( menuPoint );
	if( !dataSrcItem ){ // right click occured in "white space" of widget (not on actual item, like channel1, etc)
		connect( makeDataSrcContextMenu( dataSrcListWidget ), 
			    SIGNAL(triggered(QAction *)), 
			    this, 
			    SLOT(doDataSrcContextMenuVolAdd(QAction *)));
		
		contextMenuDataSrc->exec( dataSrcListWidget->mapToGlobal(menuPoint) );
		return; 
	}
	QVariant result  = dataSrcItem->data( USER_DATA_COL, Qt::UserRole );
	DataWrapperFactory dwf = result.value< DataWrapperFactory >(); 

	switch( dwf.getType() ){
	case CHANNEL:
		showChannelContextMenu( menuPoint );
		break;
	case SEGMENTATION:
		showSegmentationContextMenu( menuPoint );
		if( isThereASegmentationSelected() ){
			SegmentationDataWrapper sdw = getCurrentlySelectedSegmentation();
			populateSegmentElementsListWidget( sdw );	
		}
		break;
	}
}

ViewType MyInspectorWidget::getViewType( QAction *act )
{
  	if(act == xyAct) {
		return XY_VIEW;
	} else if(act == xzAct) {
		return XZ_VIEW;
	} else if (act == yzAct) {
		return YZ_VIEW;
	} else {
		throw OmFormatException("could not match QAction type...\n");
	}
}

void MyInspectorWidget::selectChannelView(QAction *act)
{
	OmId primary_id  = getCurrentlySelectedChannel().getID();
	emit triggerChannelView( primary_id, 0, 0, getViewType( act ) );
}

ChannelDataWrapper MyInspectorWidget::getCurrentlySelectedChannel()
{
	QTreeWidgetItem *dataSrcItem = dataSrcListWidget->currentItem();
	QVariant result  = dataSrcItem->data( USER_DATA_COL, Qt::UserRole );
	DataWrapperFactory dwf   = result.value< DataWrapperFactory >(); 
	return dwf.getChannelDataWrapper();
}

bool MyInspectorWidget::isThereASegmentationSelected()
{
	QTreeWidgetItem *dataSrcItem = dataSrcListWidget->currentItem();
	if( NULL == dataSrcItem ){ // no segmentations selected!
		return false;
	}

	QVariant result  = dataSrcItem->data( USER_DATA_COL, Qt::UserRole );
	DataWrapperFactory dwf = result.value< DataWrapperFactory >(); 

	if( SEGMENTATION == dwf.getType() ) {
		return true;
	}

	return false;
}

SegmentationDataWrapper MyInspectorWidget::getCurrentlySelectedSegmentation()
{
	if( !isThereASegmentationSelected() ){
		return NULL;
	}
	QTreeWidgetItem *dataSrcItem = dataSrcListWidget->currentItem();
	QVariant result  = dataSrcItem->data( USER_DATA_COL, Qt::UserRole );
	DataWrapperFactory dwf   = result.value< DataWrapperFactory >(); 
	return dwf.getSegmentationDataWrapper();
}

void MyInspectorWidget::selectSegmentationView(QAction *act)
{
	OmId primary_id  = getCurrentlySelectedSegmentation().getID();
	emit triggerSegmentationView(primary_id, 0, getViewType( act ) );
}

// called from mainwindow after segmentation or channel added
// "refresh datasrc list"
void MyInspectorWidget::addToVolume(OmManageableObject *item, ObjectType item_type)
{
	populateDataSrcListWidget();
}

void MyInspectorWidget::addFilter()
{
	ChannelDataWrapper cdw( channelInspectorWidget->getId() );

	OmFilter& added_filter = OmVolume::GetChannel( cdw.getID() ).AddFilter();

	channelInspectorWidget->setFilterID( added_filter.GetId() );

	populateDataSrcListWidget();
	populateChannelElementsListWidget( cdw );
}

void MyInspectorWidget::addSegment()
{
	SegmentationDataWrapper sdw( segInspectorWidget->getSegmentationID() );
	
	OmSegment& added_segment = OmVolume::GetSegmentation( sdw.getID() ).AddSegment();

	hashOfSementationsAndSegments.remove(  sdw.getID() );
	populateDataSrcListWidget();
	
	populateSegmentElementsListWidget( sdw );	
}

void MyInspectorWidget::refreshWidgetData()
{
	populateDataSrcListWidget();
	// TODO: update element pane, too (purcaro)
}

void MyInspectorWidget::sourceEditChangedChan()
{
	ChannelDataWrapper cdw( filObjectInspectorWidget->getChannelID() );
	
	const OmId segmenID = filObjectInspectorWidget->nameEdit->text().toInt();
	
	const OmId filterID = filObjectInspectorWidget->getFilterID();
	OmVolume::GetChannel( cdw.getID() ).GetFilter( filterID ).SetChannel( segmenID );
}

void MyInspectorWidget::sourceEditChangedSeg()
{
	ChannelDataWrapper cdw( filObjectInspectorWidget->getChannelID() );
	
	const OmId segmenID = filObjectInspectorWidget->nameEdit_2->text().toInt();
	
	const OmId filterID = filObjectInspectorWidget->getFilterID();
	OmVolume::GetChannel( cdw.getID() ).GetFilter( filterID ).SetSegmentation( segmenID );
}

void MyInspectorWidget::nameEditChanged()
{
	cout << "FIXME: purcaro: sourceEditChangedSeg\n";
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
	OmChannel &current_channel = OmVolume::GetChannel(c_id);
	
	const string &my_name = current_channel.GetName();
	channelInspectorWidget->nameEdit->setText(QString::fromStdString(my_name));
	channelInspectorWidget->nameEdit->setMinimumWidth(200);
	
	const string &my_directory = current_channel.GetSourceDirectoryPath();
	channelInspectorWidget->directoryEdit->setText(QString::fromStdString(my_directory));
	channelInspectorWidget->directoryEdit->setMinimumWidth(200);
	
	DOUT("SOURCE DIRECTORY PATH = " << my_directory);
	
	const string &my_pattern = current_channel.GetSourceFilenameRegex();
	channelInspectorWidget->patternEdit->setText(QString::fromStdString(my_pattern));
	channelInspectorWidget->patternEdit->setMinimumWidth(200);
	
	channelInspectorWidget->listWidget->clear();
	list<string>::const_iterator match_it;
	const list<string> &matches = OmVolume::GetChannel(c_id).GetSourceFilenameRegexMatches();
	
	for(match_it = matches.begin(); match_it != matches.end(); ++match_it) {
		channelInspectorWidget->listWidget->addItem(QString::fromStdString(*match_it));
	}
	
	const string &my_notes = current_channel.GetNote();
	channelInspectorWidget->notesEdit->setPlainText(QString::fromStdString(my_notes));
}

void MyInspectorWidget::populateSegmentationInspector(OmId s_id)
{
	OmSegmentation &current_segmentation = OmVolume::GetSegmentation(s_id);
	
	const string &my_name = current_segmentation.GetName();
	segInspectorWidget->nameEdit->setText(QString::fromStdString(my_name));
	segInspectorWidget->nameEdit->setMinimumWidth(200);
	
	const string &my_directory = current_segmentation.GetSourceDirectoryPath();
	segInspectorWidget->directoryEdit->setText(QString::fromStdString(my_directory));
	segInspectorWidget->directoryEdit->setMinimumWidth(200);
		
	const string &my_pattern = current_segmentation.GetSourceFilenameRegex();
	segInspectorWidget->patternEdit->setText(QString::fromStdString(my_pattern));
	segInspectorWidget->patternEdit->setMinimumWidth(200);
	
	const string &my_notes = current_segmentation.GetNote();
	segInspectorWidget->notesEdit->setPlainText(QString::fromStdString(my_notes));
	
	segInspectorWidget->listWidget->clear();

	list<string>::const_iterator match_it;
	const list<string> &matches = OmVolume::GetSegmentation(s_id).GetSourceFilenameRegexMatches();
	for(match_it = matches.begin(); match_it != matches.end(); ++match_it) {
		segInspectorWidget->listWidget->addItem(QString::fromStdString(*match_it));
	}
}

void MyInspectorWidget::populateFilterObjectInspector(OmId s_id, OmId obj_id)
{
	OmFilter &filter = OmVolume::GetChannel(s_id).GetFilter(obj_id);
	current_filter = obj_id;

	filObjectInspectorWidget->alphaSlider->setValue (filter.GetAlpha() * 100);
	filObjectInspectorWidget->nameEdit->setText (QString::number (filter.GetChannel()));
	filObjectInspectorWidget->nameEdit_2->setText (QString::number (filter.GetSegmentation()));

}

void MyInspectorWidget::populateSegmentObjectInspector(OmId s_id, OmId obj_id)
{
	segObjectInspectorWidget->setSegmentID( obj_id );
	segObjectInspectorWidget->setSegmentationID( s_id );

	OmSegment &current_obj = OmVolume::GetSegmentation(s_id).GetSegment(obj_id);
	
	const string &my_name = current_obj.GetName();
	segObjectInspectorWidget->nameEdit->setText(QString::fromStdString(my_name));
	segObjectInspectorWidget->nameEdit->setMinimumWidth(200);
	
	OmId my_id = current_obj.GetId();
	QString str;
	segObjectInspectorWidget->nameEdit_2->setText(str.setNum(my_id));
	segObjectInspectorWidget->nameEdit_2->setMinimumWidth(200);
	
	const string &my_notes = current_obj.GetNote();
	segObjectInspectorWidget->notesEdit->setPlainText(QString::fromStdString(my_notes));
	
	segObjectInspectorWidget->labelsEdit->setMinimumWidth(200);	
	
	const Vector3<float> &color = current_obj.GetColor();
	
	QPixmap *pixm = new QPixmap(40,30);
	QColor newcolor = qRgb(color.x * 255, color.y * 255, color.z * 255);
	pixm->fill(newcolor);
	
	segObjectInspectorWidget->colorButton->setIcon(QIcon(*pixm));
	
	current_color = newcolor;
}

void MyInspectorWidget::setFilAlpha(int alpha)
{
	QTreeWidgetItem *dataElementItem = dataElementsWidget->currentItem();
	QVariant result  = dataElementItem->data( USER_DATA_COL, Qt::UserRole );
	FilterDataWrapper fdw = result.value< FilterDataWrapper >(); 
	OmId item_id = fdw.getID();
	OmId channelID = fdw.getChannelID();
	
	OmVolume::GetChannel( channelID ).GetFilter( current_filter ).SetAlpha ((double) alpha / 100.00);
	OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::REDRAW));
}

void MyInspectorWidget::setSegObjColor()
{	
	DOUT("MyInspectorWidget::QColorDialog::getColor()");
	
	QColor color = QColorDialog::getColor(current_color, this);
	if (color.isValid()) {
		QPixmap *pixm = new QPixmap(40,30);
		pixm->fill(color);
		
		segObjectInspectorWidget->colorButton->setIcon(QIcon(*pixm));
		
		segObjectInspectorWidget->colorButton->update();
		current_color = color;
		
		// set the color
		
		OmId item_id = segObjectInspectorWidget->getSegmentID();
		OmId segmentationID = segObjectInspectorWidget->getSegmentationID();

		OmSegment &current_obj = OmVolume::GetSegmentation(segmentationID).GetSegment(item_id);

		Vector3<float> color_vector( color.redF(), color.greenF(), color.blueF());
		current_obj.SetColor(color_vector);
		
		//post view3d redraw event
		OmEventManager::PostEvent(new OmView3dEvent(OmView3dEvent::REDRAW));	
	}
}

void MyInspectorWidget::addChildrenToSegmentation(OmId seg_id)
{
	cout << "MyInspectorWidget::addChildrenToSegmentation: mesh was built; should we do something? (purcaro)\n";
}

void MyInspectorWidget::SegmentObjectModificationEvent(OmSegmentEvent *event)
{
	const OmId segmentationID = event->GetModifiedSegmentationId();
	if( !OmVolume::IsSegmentationValid( segmentationID ) ){
		return;
	}
	OmSegmentation &r_segmentation = OmVolume::GetSegmentation( segmentationID );
	
	OmIds selection_changed_segmentIDs = event->GetModifiedSegmentIds();
	const OmId segmentJustSelectedID   = event->GetSegmentJustSelectedID();

	if( !hashOfSementationsAndSegments.contains( segmentationID ) ){
		SegmentationDataWrapper sdw( segmentationID );
		hashOfSementationsAndSegments[ segmentationID ] = sdw.getAllSegmentIDsAndNames();
	} 
	QHash<OmId, SegmentDataWrapper> segs = hashOfSementationsAndSegments[ segmentationID ];

	foreach( OmId segID, selection_changed_segmentIDs ){
		SegmentDataWrapper seg = segs[ segID ];
		seg.setSelected( r_segmentation.IsSegmentSelected( segID ) );
		segs[ segID ] = seg;
	}
					
	hashOfSementationsAndSegments[ segmentationID ] = segs;

	// TODO: this is wrong; switch to corrent segmentation and segment list instead 
	// TODO: also, jump to right segmentation list...
	if( isThereASegmentationSelected() ){
		SegmentationDataWrapper sdw = getCurrentlySelectedSegmentation();
		populateSegmentElementsListWidget( sdw );	
	}
}


//	(new OmVoxelSetValueAction(segmentation_id, picked_voxel, data_value))->Run();
 

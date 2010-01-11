#ifndef MYINSPECTORWIDGET_H
#define MYINSPECTORWIDGET_H

#include <QWidget> 
//#include "ui_myInspectorWidget.h" 

#include <QtGui/QSplitter>
#include <QtGui/QItemSelection>

#include "common/omStd.h"
#include "system/omSystemTypes.h"
#include "system/omFilter.h"
#include "filObjectInspector.h"

#include "system/events/omSegmentEvent.h"

#include "dataWrappers.h"
#include "inspectorHelpers/segmentationHelper.h"
#include "inspectorHelpers/channelHelper.h"

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QTreeWidget>
#include <QtGui/QSpacerItem>
#include <QtGui/QSplitter>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>


class QTreeView;
class InspectorTreeView;
class TreeProxyModel;
class TreeModel;

class SegInspector;
class VolumeInspector;
class ChanInspector;
class SegObjectInspector;

class PreferencesMesh;
class Preferences2d;
class Preferences3d;

class OmManageableObject;


class MyInspectorWidget : public QWidget, public OmSegmentEventListener
{ 
    Q_OBJECT 
	    
  public: 
	friend class SegmentationHelper;
	friend class ChannelHelper;

    MyInspectorWidget(QWidget *parent = 0); 
    ~MyInspectorWidget();	
	
	void addToVolume(OmManageableObject *item, ObjectType item_type);
	void addToSegmentation();
	void addToChannel();

	void refreshWidgetData();
	
signals:
	void treeDataChanged();
	void addChannel();
	void addSegmentation();
	
	void triggerChannelView(OmId chan_id, OmId second_chan_id, OmId third_chan_id, ViewType vtype);
	void triggerSegmentationView(OmId primary_id, OmId secondary_id, ViewType vtype);
	
	
protected:
	void SegmentObjectModificationEvent(OmSegmentEvent *event);
	void SegmentSelectionChangeEvent(OmSegmentEvent *event);
	
private slots:
	void addFilter ();
	void addPreferencesToSplitter(QTreeWidgetItem *item, const int column );
	void addSegment();
	void nameEditChanged();
	void setSegObjColor();
	void setFilAlpha(int);
	void addChildrenToSegmentation(OmId seg_id);

	void showDataSrcContextMenu(const QPoint& menuPoint);
	
	void selectChannelView(QAction *act);
	void selectSegmentationView(QAction *act);
	void sourceEditChangedChan ();
	void sourceEditChangedSeg ();
	void addToSplitterDataElementSegment( QTreeWidgetItem * current, const int column );
	void addToSplitterDataElementFilter( QTreeWidgetItem * current, const int column );
	void addToSplitterDataSource( QTreeWidgetItem * current, const int column );

	void rebuildSegmentList( const OmId segmentationID );
	void doDataSrcContextMenuVolAdd( QAction *act );

private:
    
	QSplitter *splitter;

	///////////////////////////////
	// new inspector elements
	QTreeWidget *dataSrcListWidget;
	void populateDataSrcListWidget();
	void addChannelToSplitter(  ChannelDataWrapper data );
	void addSegmentationToSplitter( SegmentationDataWrapper data );

	void showChannelContextMenu( const QPoint& menuPoint );
	void showSegmentationContextMenu( const QPoint& menuPoint );
	QMenu* makeContextMenuBase( const QPoint& menuPoint, QTreeWidget* parent  );
	bool isThereASegmentationSelected();

	QTreeWidget *dataElementsWidget;
	QTabWidget *dataElementsTabs;
	void populateSegmentElementsListWidget( SegmentationDataWrapper data );
	void populateChannelElementsListWidget( ChannelDataWrapper data );
	QTreeWidget* setupVolumeList( QWidget *layoutWidget );
	QTreeWidget* setupDataElementList();
	QTreeWidget* setupDataSrcList();
	ChannelDataWrapper getCurrentlySelectedChannel();
	SegmentationDataWrapper getCurrentlySelectedSegmentation();
	Qt::CheckState getCheckState( const bool enabled );
	void setRowFlagsAndCheckState( QTreeWidgetItem *row, Qt::CheckState checkState );

	SegmentationHelper* segmenHelper;
	ChannelHelper* channelHelper;

	QMenu* makeDataSrcContextMenu( QTreeWidget* parent  );
	void addChannelToVolume();
	void addSegmentationToVolume();

	// keep local hash of segmentation --> segments to maintain 
	//  GUI state information about which segments are selected
	QHash<OmId, QHash<OmId, SegmentDataWrapper> > hashOfSementationsAndSegments;
	///////////////////////////////

	QAction *xyAct;
	QAction *xzAct;
	QAction *yzAct;

	QAction *addChannelAct;
	QAction *addSegmentationAct;
	
	QMenu *contextMenu;
	QMenu *contextMenuDataSrc;

	SegInspector *segInspectorWidget;
	ChanInspector *channelInspectorWidget;
	VolumeInspector *volumeInspectorWidget;
	SegObjectInspector *segObjectInspectorWidget;
	FilObjectInspector *filObjectInspectorWidget;
	
	Preferences2d *preferences2dWidget;
	Preferences3d *preferences3dWidget;
	PreferencesMesh *preferencesMeshWidget;
	
	void populateChannelInspector(OmId c_id);
	void populateSegmentationInspector( OmId s_id);
	void populateSegmentObjectInspector(OmId s_id, OmId obj_id);
	void populateFilterObjectInspector(OmId s_id, OmId obj_id);
	
	QColor current_color;
	OmId current_filter;

	int current_object;
	bool preferencesActivated;
	
	bool first_access;
	bool iSentIt;

	ViewType getViewType( QAction *act );
};

#endif

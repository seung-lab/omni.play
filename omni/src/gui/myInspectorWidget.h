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

#include "utility/dataWrappers.h"
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

class SegInspector;
class VolumeInspector;
class ChanInspector;
class SegObjectInspector;

class PreferencesMesh;
class Preferences2d;
class Preferences3d;

class MyInspectorWidget:public QWidget, public OmSegmentEventListener {
 Q_OBJECT public:
	friend class SegmentationHelper;
	friend class ChannelHelper;

	 MyInspectorWidget(QWidget * parent = 0);
	~MyInspectorWidget();

	void addToVolume(OmManageableObject * item, ObjectType item_type);
	void addToSegmentation();
	void addToChannel();

	void refreshWidgetData();

 signals: 
	void addChannel();
	void addSegmentation();

	void triggerChannelView(OmId chan_id, OmId second_chan_id, OmId third_chan_id, ViewType vtype);
	void triggerSegmentationView(OmId primary_id, OmId secondary_id, ViewType vtype);

 protected:

	static const int ENABLED_COL = 0;
	static const int NAME_COL = 1;
	static const int ID_COL = 2;
	static const int NOTE_COL = 3;
	static const int USER_DATA_COL = 4;
	static const int MAX_COL_TO_DISPLAY = 3;

	void SegmentObjectModificationEvent(OmSegmentEvent * event);

 private slots: 
        void addFilter();
	void addSegment();
	void nameEditChanged();
	void setSegObjColor();
	void setFilAlpha(int);
	void addChildrenToSegmentation(OmId seg_id);

	void showDataSrcContextMenu(const QPoint & menuPoint);
	void leftClickOnDataSourceItem(QTreeWidgetItem * current, const int column);

	void selectChannelView(QAction * act);
	void selectSegmentationView(QAction * act);
	void sourceEditChangedChan();
	void sourceEditChangedSeg();
	void addToSplitterDataElementSegment(QTreeWidgetItem * current, const int column);
	void addToSplitterDataElementFilter(QTreeWidgetItem * current, const int column);
	void addToSplitterDataSource(QTreeWidgetItem * current, const int column);

	void rebuildSegmentList(const OmId segmentationID);
	void doDataSrcContextMenuVolAdd(QAction * act);
	void leftClickOnSegment(QTreeWidgetItem * current, const int column);

 private:

	 QSplitter * splitter;

	///////////////////////////////
	// new inspector elements
	QTreeWidget *dataSrcListWidget;
	void populateDataSrcListWidget();
	void addChannelToSplitter(ChannelDataWrapper data);
	void addSegmentationToSplitter(SegmentationDataWrapper data);

	void showChannelContextMenu();
	void showSegmentationContextMenu();
	void doShowDataSrcContextMenu( QTreeWidgetItem *dataSrcItem );
	QMenu *makeContextMenuBase(QTreeWidget * parent);
	bool isThereASegmentationSelected();

	QTreeWidget *dataElementsWidget;
	QTabWidget *dataElementsTabs;
	void populateSegmentElementsListWidget(const bool doScrollToSelectedSegment =
					       false, const OmId segmentJustSelectedID = 0);
	void populateChannelElementsListWidget(ChannelDataWrapper data);
	QTreeWidget *setupVolumeList(QWidget * layoutWidget);
	QTreeWidget *setupDataElementList();
	QTreeWidget *setupDataSrcList();
	ChannelDataWrapper getCurrentlySelectedChannel();	// TODO: fixme (purcaro)
	 Qt::CheckState getCheckState(const bool enabled);
	void setRowFlagsAndCheckState(QTreeWidgetItem * row, Qt::CheckState checkState);

	SegmentationHelper *segmenHelper;
	ChannelHelper *channelHelper;

	QMenu *makeDataSrcContextMenu(QTreeWidget * parent);
	void addChannelToVolume();
	void addSegmentationToVolume();
	void autoResizeColumnWidths(QTreeWidget * widget);
	void makeSegmentationActive(const OmId segmentationID);
	void makeSegmentationActive(SegmentationDataWrapper sdw);
	void makeSegmentationActive(const OmId segmentationID, const OmId segmentJustSelectedID);
	void makeSegmentationActive(SegmentationDataWrapper sdw, const OmId segmentJustSelectedID);
	void sendSegmentChangeEvent(SegmentDataWrapper sdw, const bool augment_selection);
	void rebuildSegmentList(const OmId segmentationID, const OmId segmentJustAddedID);
	DataWrapperContainer currentDataSrc;

	// keep local hash of segmentation --> segments to maintain 
	//  GUI state information about which segments are selected
	 QHash < OmId, QHash < OmId, SegmentDataWrapper > >hashOfSementationsAndSegments;
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

	void populateChannelInspector(OmId c_id);
	void populateSegmentationInspector(OmId s_id);
	void populateSegmentObjectInspector(OmId s_id, OmId obj_id);
	void populateFilterObjectInspector(OmId s_id, OmId obj_id);

	QColor current_color;

	int current_object;

	bool first_access;

	ViewType getViewType(QAction * act);
};

#endif

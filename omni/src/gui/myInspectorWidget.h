#ifndef MYINSPECTORWIDGET_H
#define MYINSPECTORWIDGET_H

#include <QtCore/QVariant>
#include <QtGui>
#include <QWidget>

#include "inspectors/segInspector.h"
#include "inspectors/chanInspector.h"
#include "inspectors/segObjectInspector.h"
#include "inspectors/filObjectInspector.h"
#include "inspectors/inspectorProperties.h"

#include "common/omStd.h"
#include "system/omSystemTypes.h"
#include "volume/omFilter.h"
#include "system/events/omSegmentEvent.h"
#include "utility/dataWrappers.h"

class SegInspector;
class ChanInspector;
class SegObjectInspector;

class MyInspectorWidget : public QWidget, public OmSegmentEventListener {
 Q_OBJECT 
 
 public:
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

	void SegmentObjectModificationEvent(OmSegmentEvent * event);

 private slots: 
        void addFilter();
	void addSegment();
	void nameEditChanged();
	void addChildrenToSegmentation(OmId seg_id);

	void showDataSrcContextMenu(const QPoint & menuPoint);
	void leftClickOnDataSourceItem(QTreeWidgetItem * current, const int column);
	void leftClickOnFilterItem(QTreeWidgetItem * current, const int column);

	void selectChannelView(QAction * act);
	void selectSegmentationView(QAction * act);
	void addToSplitterDataElementSegment(QTreeWidgetItem * current, const int column);
	void addToSplitterDataElementFilter(QTreeWidgetItem * current, const int column);
	void addToSplitterDataSource(QTreeWidgetItem * current, const int column);

	void rebuildSegmentList(const OmId segmentationID);
	void doDataSrcContextMenuVolAdd(QAction * act);
	void leftClickOnSegment(QTreeWidgetItem * current, const int column);

 private:

	///////////////////////////////
	// new inspector elements
	QTreeWidget *dataSrcListWidget;
	QTreeWidget *filterListWidget;
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
	void populateFilterListWidget(ChannelDataWrapper cdw);
	QTreeWidget * setupVolumeList(QWidget * layoutWidget);
	QTreeWidget * setupDataElementList();
	QTreeWidget * setupDataSrcList();
	QTreeWidget * setupFilterList();
	ChannelDataWrapper getCurrentlySelectedChannel();
	Qt::CheckState getCheckState(const bool enabled);
	bool getBoolState(const Qt::CheckState state);
	void setRowFlagsAndCheckState(QTreeWidgetItem * row, Qt::CheckState checkState);

	QMenu *makeDataSrcContextMenu(QTreeWidget * parent);
	void addChannelToVolume();
	void addSegmentationToVolume();
	void autoResizeColumnWidths(QTreeWidget * widget, const int max_col_to_display);
	void makeSegmentationActive(const OmId segmentationID);
	void makeSegmentationActive(SegmentationDataWrapper sdw);
	void makeSegmentationActive(const OmId segmentationID, const OmId segmentJustSelectedID);
	void makeSegmentationActive(SegmentationDataWrapper sdw, const OmId segmentJustSelectedID);
	void sendSegmentChangeEvent(SegmentDataWrapper sdw, const bool augment_selection);
	void rebuildSegmentList(const OmId segmentationID, const OmId segmentJustAddedID);
	DataWrapperContainer currentDataSrc;

	QWidget *layoutWidget;

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
	SegObjectInspector *segObjectInspectorWidget;
	FilObjectInspector *filObjectInspectorWidget;

	void populateChannelInspector(OmId c_id);
	void populateSegmentationInspector( SegmentationDataWrapper sdw);

	InspectorProperties * inspectorProperties;

	ViewType getViewType(QAction * act);

	void setTabEnabled( QWidget * tab, QString title );
};

#endif

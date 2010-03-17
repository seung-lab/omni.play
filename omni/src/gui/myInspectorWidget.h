#ifndef MYINSPECTORWIDGET_H
#define MYINSPECTORWIDGET_H

#include <QtCore/QVariant>
#include <QtGui>
#include <QWidget>

#include "gui/segmentList.h"
#include "gui/elementListBox.h"
#include "inspectors/segInspector.h"
#include "inspectors/chanInspector.h"
#include "inspectors/filObjectInspector.h"
#include "inspectors/inspectorProperties.h"

#include "common/omStd.h"
#include "system/omSystemTypes.h"
#include "volume/omFilter.h"
#include "system/events/omSegmentEvent.h"
#include "utility/dataWrappers.h"

class SegInspector;
class ChanInspector;

class MyInspectorWidget : public QWidget, public OmSegmentEventListener {
 Q_OBJECT 
 
 public:
	 MyInspectorWidget(QWidget * parent);
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
	void nameEditChanged();
	void addChildrenToSegmentation(OmId seg_id);

	void showDataSrcContextMenu(const QPoint & menuPoint);
	void leftClickOnDataSourceItem(QTreeWidgetItem * current, const int column);
	void leftClickOnFilterItem(QTreeWidgetItem * current, const int column);

	void selectChannelView(QAction * act);
	void selectSegmentationView(QAction * act);
	void addToSplitterDataElementFilter(QTreeWidgetItem * current, const int column);
	void addToSplitterDataSource(QTreeWidgetItem * current);

	void doDataSrcContextMenuVolAdd(QAction * act);
	void addSegment();

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


	void populateFilterListWidget(ChannelDataWrapper cdw);
	QTreeWidget * setupVolumeList(QWidget * layoutWidget);

	QTreeWidget * setupDataSrcList();
	QTreeWidget * setupFilterList();
	ChannelDataWrapper getCurrentlySelectedChannel();
	SegmentationDataWrapper getCurrentlySelectedSegmentation();
	void setRowFlagsAndCheckState(QTreeWidgetItem * row, Qt::CheckState checkState);

	QMenu *makeDataSrcContextMenu(QTreeWidget * parent);
	void addChannelToVolume();
	void addSegmentationToVolume();

	DataWrapperContainer currentDataSrc;

	///////////////////////////////

	QAction *xyAct;
	QAction *xzAct;
	QAction *yzAct;
	QAction *propAct;

	QAction *addChannelAct;
	QAction *addSegmentationAct;

	QMenu *contextMenu;
	QMenu *contextMenuDataSrc;

	SegInspector *segInspectorWidget;
	ChanInspector *channelInspectorWidget;
	FilObjectInspector *filObjectInspectorWidget;

	void populateChannelInspector(OmId c_id);
	void populateSegmentationInspector( SegmentationDataWrapper sdw);

	InspectorProperties * inspectorProperties;
	SegmentList * segmentList;
	ElementListBox * elementListBox;

	ViewType getViewType(QAction * act);

	QVBoxLayout * verticalLayout;
};

#endif

#ifndef MYINSPECTORWIDGET_H
#define MYINSPECTORWIDGET_H

#include <QtCore/QVariant>
#include <QtGui>
#include <QWidget>

#include "gui/mainwindow.h"
#include "gui/segmentList.h"
#include "gui/validList.h"
#include "gui/elementListBox.h"
#include "inspectors/segInspector.h"
#include "inspectors/chanInspector.h"
#include "inspectors/filObjectInspector.h"
#include "inspectors/inspectorProperties.h"

#include "common/omStd.h"
#include "volume/omFilter2d.h"
#include "system/events/omSegmentEvent.h"
#include "utility/dataWrappers.h"

class SegInspector;
class ChanInspector;

class MyInspectorWidget : public QWidget, public OmSegmentEventListener {
 Q_OBJECT 
 
 public:
	 MyInspectorWidget( MainWindow* parent);
	~MyInspectorWidget();

	void addChannelToVolume();
	void addSegmentationToVolume();

	void refreshWidgetData();

 signals: 
	void triggerChannelView(OmId chan_id, ViewType vtype);
	void triggerSegmentationView(OmId primary_id, ViewType vtype);

 protected:

	static const int ENABLED_COL = 0;
	static const int NAME_COL = 1;
	static const int ID_COL = 2;
	static const int NOTE_COL = 3;
	static const int USER_DATA_COL = 4;

	void SegmentObjectModificationEvent(OmSegmentEvent * event);
	void SegmentDataModificationEvent() {}
	void SegmentEditSelectionChangeEvent() {}

 private slots: 
        void addFilter();
	void nameEditChanged();

	void showDataSrcContextMenu(const QPoint & menuPoint);
	void leftClickOnDataSourceItem(QTreeWidgetItem * current);
	void leftClickOnFilterItem(QTreeWidgetItem * current);

	void selectChannelView(QAction * act);
	void selectSegmentationView(QAction * act);
	void addToSplitterDataElementFilter(QTreeWidgetItem * current);
	void addToSplitterDataSource(QTreeWidgetItem * current);

	void doDataSrcContextMenuVolAdd(QAction * act);
	void addSegment();
	void deleteSegmentation(SegmentationDataWrapper sdw);
	void deleteChannel(ChannelDataWrapper cdw);
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

	void populateFilterListWidget(ChannelDataWrapper cdw);

	QTreeWidget * setupDataSrcList();
	QTreeWidget * setupFilterList();
	ChannelDataWrapper getCurrentlySelectedChannel();
	SegmentationDataWrapper getCurrentlySelectedSegmentation();
	void setRowFlagsAndCheckState(QTreeWidgetItem * row, Qt::CheckState checkState);

	QMenu *makeDataSrcContextMenu(QTreeWidget * parent);

	DataWrapperContainer currentDataSrc;

	MainWindow* mParentWindow;

	///////////////////////////////

	QAction *xyAct;
	QAction *xzAct;
	QAction *yzAct;
	QAction *propAct;
	QAction *delAct;

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
	ValidList * validList;
	ElementListBox * elementListBox;

	ViewType getViewType(QAction * act);

	QVBoxLayout * verticalLayout;
	QString getSegmentationGroupBoxTitle(SegmentationDataWrapper sdw);
	
};

#endif

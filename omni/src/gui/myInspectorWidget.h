#ifndef MYINSPECTORWIDGET_H
#define MYINSPECTORWIDGET_H

#include <QtCore/QVariant>
#include <QtGui>
#include <QWidget>

#include "common/omCommon.h"
#include "zi/omUtility.h"

class ChanInspector;
class ChannelDataWrapper;
class DataWrapperContainer;
class ElementListBox;
class FilObjectInspector;
class InspectorProperties;
class MainWindow;
class OmViewGroupState;
class SegInspector;
class SegmentationDataWrapper;

class MyInspectorWidget : public QWidget {
    Q_OBJECT

public:
    MyInspectorWidget( MainWindow* parent, OmViewGroupState * vgs);
    ~MyInspectorWidget();

    OmViewGroupState* getViewGroupState(){ return mViewGroupState; }

    void addChannelToVolume();
    void addSegmentationToVolume();

    void refreshWidgetData();
    void rebuildSegmentLists(const OmID segmentationID, const OmSegID segID);

Q_SIGNALS:
    void triggerChannelView(OmID chan_id, ViewType vtype);
    void triggerSegmentationView(OmID primary_id, ViewType vtype);

protected:
    static const int ENABLED_COL = 0;
    static const int NAME_COL = 1;
    static const int ID_COL = 2;
    static const int NOTE_COL = 3;
    static const int USER_DATA_COL = 4;

private Q_SLOTS:
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

private:

    QTreeWidget *dataSrcListWidget;
    QTreeWidget *filterListWidget;

    void deleteSegmentation(SegmentationDataWrapper sdw);
    void deleteChannel(ChannelDataWrapper cdw);

    void populateDataSrcListWidget();
    void addChannelToSplitter(ChannelDataWrapper data);
    void addSegmentationToSplitter(SegmentationDataWrapper data);

    void showChannelContextMenu();
    void showSegmentationContextMenu();
    void doShowDataSrcContextMenu( QTreeWidgetItem *dataSrcItem );
    QMenu* makeContextMenuBase(QTreeWidget * parent);
    QMenu* makeSegmentationContextMenu(QTreeWidget* parent);

    void populateFilterListWidget(ChannelDataWrapper cdw);

    QTreeWidget * setupDataSrcList();
    QTreeWidget * setupFilterList();
    ChannelDataWrapper getCurrentlySelectedChannel();
    SegmentationDataWrapper getCurrentlySelectedSegmentation();
    void setRowFlagsAndCheckState(QTreeWidgetItem * row, Qt::CheckState checkState);

    QMenu *makeDataSrcContextMenu(QTreeWidget * parent);

    boost::shared_ptr<DataWrapperContainer> currentDataSrc;

    MainWindow* mParentWindow;

    QAction *xyAct;
    QAction *xzAct;
    QAction *yzAct;
    QAction *propAct;
    QAction *delAct;
    QAction *examMSTAct;

    QAction *addChannelAct;
    QAction *addSegmentationAct;

    QMenu *contextMenu;
    QMenu *contextMenuDataSrc;

    SegInspector *segInspectorWidget;
    ChanInspector *channelInspectorWidget;
    FilObjectInspector *filObjectInspectorWidget;

    void populateChannelInspector(OmID c_id);
    void populateSegmentationInspector( SegmentationDataWrapper sdw);

    InspectorProperties * inspectorProperties;

    ViewType getViewType(QAction * act);

    QVBoxLayout * verticalLayout;
    void updateSegmentListBox( SegmentationDataWrapper sdw );

    OmViewGroupState* mViewGroupState;

    void showMSTtable(SegmentationDataWrapper sdw);
};

#endif

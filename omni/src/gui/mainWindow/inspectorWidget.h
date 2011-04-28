#ifndef MYINSPECTORWIDGET_H
#define MYINSPECTORWIDGET_H

#include "common/omCommon.h"
#include "zi/omUtility.h"

#include <QtCore/QVariant>
#include <QtGui>
#include <QWidget>

class ChanInspector;
class ChannelDataWrapper;
class ElementListBox;
class FilObjectInspector;
class InspectorProperties;
class MainWindow;
class OmViewGroupState;
class SegmentationDataWrapper;

class InspectorWidget : public QWidget {
Q_OBJECT

public:
    InspectorWidget(QWidget* parent, MainWindow* mainWindow, OmViewGroupState* vgs);
    ~InspectorWidget();

    OmViewGroupState* getViewGroupState(){
        return vgs_;
    }

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
    void leftClickOnDataSourceItem(QTreeWidgetItem* current);
    void leftClickOnFilterItem(QTreeWidgetItem* current);

    void selectChannelView(QAction* act);
    void selectSegmentationView(QAction* act);
    void addToSplitterDataElementFilter(QTreeWidgetItem* current);
    void addToSplitterDataSource(QTreeWidgetItem* current);

    void doDataSrcContextMenuVolAdd(QAction* act);

    void openChannelView(OmID chan_id, ViewType vtype);
    void openSegmentationView(OmID primary_id, ViewType vtype);

private:
    MainWindow* const mainWindow_;
    OmViewGroupState* const vgs_;

    QTreeWidget* dataSrcListWidget;
    QTreeWidget* filterListWidget;

    void deleteSegmentation(SegmentationDataWrapper sdw);
    void deleteChannel(ChannelDataWrapper cdw);

    void populateDataSrcListWidget();
    void addChannelToSplitter(ChannelDataWrapper data);
    void addSegmentationToSplitter(SegmentationDataWrapper data);

    void showChannelContextMenu();
    void showSegmentationContextMenu();
    void doShowDataSrcContextMenu( QTreeWidgetItem* dataSrcItem );
    QMenu* makeContextMenuBase(QTreeWidget* parent);
    QMenu* makeSegmentationContextMenu(QTreeWidget* parent);

    void populateFilterListWidget(ChannelDataWrapper cdw);

    QTreeWidget* setupDataSrcList();
    QTreeWidget* setupFilterList();
    ChannelDataWrapper getCurrentlySelectedChannel();
    SegmentationDataWrapper getCurrentlySelectedSegmentation();
    void setRowFlagsAndCheckState(QTreeWidgetItem* row, Qt::CheckState checkState);

    QMenu* makeDataSrcContextMenu(QTreeWidget* parent);

    QAction* xyAct;
    QAction* xzAct;
    QAction* yzAct;
    QAction* propAct;
    QAction* delAct;
    QAction* examMSTAct;

    QAction* addChannelAct;
    QAction* addSegmentationAct;

    QMenu* contextMenu;
    QMenu* contextMenuDataSrc;

    ChanInspector* channelInspectorWidget;
    FilObjectInspector* filObjectInspectorWidget;

    void populateChannelInspector(OmID c_id);
    void populateSegmentationInspector( SegmentationDataWrapper sdw);

    InspectorProperties* inspectorProperties_;

    ViewType getViewType(QAction* act);

    QVBoxLayout* verticalLayout;
    void updateSegmentListBox( SegmentationDataWrapper sdw );

    void showMSTtable(SegmentationDataWrapper sdw);
};

#endif

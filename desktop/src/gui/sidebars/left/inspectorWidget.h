#pragma once

#include "common/common.h"
#include "zi/omUtility.h"

#include <QtCore/QVariant>
#include <QtGui>
#include <QWidget>

class ChannelInspector;
class ChannelDataWrapper;
class ElementListBox;
class FilObjectInspector;
class InspectorProperties;
class MainWindow;
class OmViewGroupState;
class SegmentationDataWrapper;

class InspectorWidget : public QWidget {
  Q_OBJECT;
  ;
 public:
  InspectorWidget(QWidget* parent, MainWindow* mainWindow,
                  OmViewGroupState* vgs);
  ~InspectorWidget();

  OmViewGroupState* GetViewGroupState() { return vgs_; }

  void addChannelToVolume();
  void addSegmentationToVolume();
  void addAffinityToVolume();

  void refreshWidgetData();
  void rebuildSegmentLists(const om::common::ID segmentationID,
                           const om::common::SegID segID);

Q_SIGNALS:
  void triggerChannelView(om::common::ID chan_id, om::common::ViewType vtype);
  void triggerSegmentationView(om::common::ID primary_id,
                               om::common::ViewType vtype);

 protected:
  static const int ENABLED_COL = 0;
  static const int NAME_COL = 1;
  static const int ID_COL = 2;
  static const int NOTE_COL = 3;
  static const int USER_DATA_COL = 4;

 private
Q_SLOTS:
  void nameEditChanged();

  void showDataSrcContextMenu(const QPoint& menuPoint);
  void leftClickOnDataSourceItem(QTreeWidgetItem* current);
  void leftClickOnFilterItem(QTreeWidgetItem* current);

  void selectChannelView(QAction* act);
  void selectSegmentationView(QAction* act);
  void addToSplitterDataElementFilter(QTreeWidgetItem* current);
  void addToSplitterDataSource(QTreeWidgetItem* current);

  void doDataSrcContextMenuVolAdd(QAction* act);

  void openChannelView(om::common::ID chan_id, om::common::ViewType vtype);
  void openSegmentationView(om::common::ID primary_id,
                            om::common::ViewType vtype);

 private:
  MainWindow* const mainWindow_;
  OmViewGroupState* const vgs_;

  QTreeWidget* dataSrcListWidget_;
  QTreeWidget* filterListWidget_;

  void deleteSegmentation(SegmentationDataWrapper sdw);
  void deleteChannel(ChannelDataWrapper cdw);

  void populateDataSrcListWidget();
  void addChannelToSplitter(ChannelDataWrapper data);
  void addSegmentationToSplitter(SegmentationDataWrapper data);

  void showChannelContextMenu();
  void showSegmentationContextMenu();
  void showAffinityContextMenu();
  void doShowDataSrcContextMenu(QTreeWidgetItem* dataSrcItem);
  QMenu* makeContextMenuBase(QTreeWidget* parent);
  QMenu* makeSegmentationContextMenu(QTreeWidget* parent);

  void populateFilterListWidget(ChannelDataWrapper cdw);

  QTreeWidget* setupDataSrcList();
  QTreeWidget* setupFilterList();
  ChannelDataWrapper getCurrentlySelectedChannel();
  SegmentationDataWrapper getCurrentlySelectedSegmentation();
  void setRowFlagsAndCheckState(QTreeWidgetItem* row,
                                Qt::CheckState checkState);

  QMenu* makeDataSrcContextMenu(QTreeWidget* parent);

  QAction* xyAct_;
  QAction* xzAct_;
  QAction* yzAct_;
  QAction* propAct_;
  QAction* newView3dAct_;
  QAction* delAct_;
  QAction* examMSTAct_;

  QAction* addChannelAct_;
  QAction* addSegmentationAct_;
  QAction* addAffinityAct_;

  QMenu* contextMenu_;
  QMenu* contextMenuDataSrc_;

  ChannelInspector* channelInspectorWidget_;
  FilObjectInspector* filObjectInspectorWidget_;

  void populateChannelInspector(om::common::ID c_id);
  void populateSegmentationInspector(SegmentationDataWrapper sdw);

  InspectorProperties* inspectorProperties_;

  om::common::ViewType getViewType(QAction* act);

  void updateSegmentListBox(SegmentationDataWrapper sdw);

  void showMSTtable(SegmentationDataWrapper sdw);
  void showNewView3d();
};

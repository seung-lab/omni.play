#include "common/omDebug.h"
#include "gui/guiUtils.hpp"
#include "gui/inspectors/chanInspector.h"
#include "gui/inspectors/filObjectInspector.h"
#include "gui/inspectors/inspectorProperties.h"
#include "gui/inspectors/segmentation/segmentationInspector.h"
#include "gui/mainWindow/inspectorWidget.h"
#include "gui/mainWindow/mainWindow.h"
#include "gui/mstViewer.hpp"
#include "gui/segmentLists/elementListBox.hpp"
#include "gui/widgets/omAskQuestion.hpp"
#include "segment/omSegment.h"
#include "segment/omSegments.h"
#include "system/omAppState.hpp"
#include "system/omConnect.hpp"
#include "utility/dataWrappers.h"
#include "viewGroup/omViewGroupState.h"

#include <QtGui>
#include <QMessageBox>

Q_DECLARE_METATYPE(DataWrapperContainer);
Q_DECLARE_METATYPE(FilterDataWrapper);

InspectorWidget::InspectorWidget(QWidget* parent, MainWindow* mainWindow, OmViewGroupState* vgs)
    : QWidget(parent)
    , mainWindow_(mainWindow)
    , vgs_(vgs)
{
    verticalLayout = new QVBoxLayout(this);

    verticalLayout->addWidget(setupDataSrcList());

    inspectorProperties = new InspectorProperties(this, vgs_);

    ElementListBox::Create(vgs_);
    verticalLayout->addWidget(ElementListBox::Widget());

    channelInspectorWidget = NULL;
    segmentationInspectorWidget = NULL;

    QMetaObject::connectSlotsByName(this);

    om::connect(this, SIGNAL(triggerChannelView(OmID, ViewType)),
                this, SLOT(openChannelView(OmID, ViewType)));

    om::connect(this, SIGNAL(triggerSegmentationView(OmID, ViewType)),
                    this, SLOT(openSegmentationView(OmID, ViewType)));

    OmAppState::SetInspector(this);
}

InspectorWidget::~InspectorWidget()
{
    ElementListBox::Delete();
    OmAppState::SetInspector(NULL);
}

void InspectorWidget::openChannelView(OmID chan_id, ViewType vtype){
        vgs_->addView2Dchannel(chan_id, vtype);
}

void InspectorWidget::openSegmentationView(OmID segmentation_id, ViewType vtype){
        vgs_->addView2Dsegmentation(segmentation_id, vtype);
}

QTreeWidget* InspectorWidget::setupFilterList()
{
    filterListWidget = new QTreeWidget(this);
    filterListWidget->setAlternatingRowColors(false);
    filterListWidget->setColumnCount(3);

    QStringList headers;
    headers << tr("enabled") << tr("Name") << tr("ID") << tr("Notes");
    filterListWidget->setHeaderLabels(headers);

    return filterListWidget;
}

void InspectorWidget::setRowFlagsAndCheckState(QTreeWidgetItem* row, Qt::CheckState checkState)
{
    row->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    row->setCheckState(ENABLED_COL, checkState);
}

void InspectorWidget::populateDataSrcListWidget()
{
    dataSrcListWidget->clear();

    Q_FOREACH(OmID channID, ChannelDataWrapper::ValidIDs())
    {
        DataWrapperContainer dwc = DataWrapperContainer(CHANNEL, channID);
        ChannelDataWrapper cdw = dwc.getChannelDataWrapper();
        QTreeWidgetItem *row = new QTreeWidgetItem(dataSrcListWidget);
        row->setText(NAME_COL, cdw.getName());
        row->setText(ID_COL, QString("%1").arg(cdw.getID()));
        row->setText(NOTE_COL, cdw.getNote());
        row->setData(USER_DATA_COL, Qt::UserRole, qVariantFromValue(dwc));
        setRowFlagsAndCheckState(row, GuiUtils::getCheckState(cdw.isEnabled()));
    }

    Q_FOREACH(OmID segmenID, SegmentationDataWrapper::ValidIDs())
    {
        DataWrapperContainer dwc = DataWrapperContainer(SEGMENTATION, segmenID);
        SegmentationDataWrapper sdw = dwc.GetSDW();
        QTreeWidgetItem *row = new QTreeWidgetItem(dataSrcListWidget);
        row->setText(NAME_COL, sdw.GetName());
        row->setText(ID_COL, QString("%1").arg(sdw.getID()));
        row->setText(NOTE_COL, sdw.getNote());
        row->setData(USER_DATA_COL, Qt::UserRole, qVariantFromValue(dwc));
        setRowFlagsAndCheckState(row, GuiUtils::getCheckState(sdw.isEnabled()));
    }

    dataSrcListWidget->disconnect(SIGNAL(itemClicked(QTreeWidgetItem *, int)));

    om::connect(dataSrcListWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
                this, SLOT(leftClickOnDataSourceItem(QTreeWidgetItem *)));

    GuiUtils::autoResizeColumnWidths(dataSrcListWidget, 3);

    dataSrcListWidget->update();
}

void InspectorWidget::populateFilterListWidget(ChannelDataWrapper cdw)
{
    ElementListBox::Reset();
    ElementListBox::SetTitle(QString("Channel %1").arg(cdw.getID()));
    ElementListBox::AddTab(0, setupFilterList(), "Filters");

    filterListWidget->clear();

    filterListWidget->selectionModel()->blockSignals(true);
    filterListWidget->selectionModel()->clearSelection();

    std::vector<OmFilter2d*> filters = cdw.GetFilters();

    FOR_EACH(iter, filters){
        OmFilter2d* filterPtr = *iter;
        FilterDataWrapper filter(cdw.getID(), filterPtr->GetID());
        QTreeWidgetItem *row = new QTreeWidgetItem(filterListWidget);
        row->setText(NAME_COL, filter.getName());
        row->setText(ID_COL, QString("%1").arg(filter.getID()));
        row->setData(USER_DATA_COL, Qt::UserRole, qVariantFromValue(filter));
        //row->setText(NOTE_COL, filter->getNote());
        row->setText(NOTE_COL, "");
        setRowFlagsAndCheckState(row, GuiUtils::getCheckState(true));
        //row->setSelected(seg.isSelected());
    }

    filterListWidget->selectionModel()->blockSignals(false);

    filterListWidget->disconnect(SIGNAL(itemClicked(QTreeWidgetItem *, int)));
    om::connect(filterListWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
                this, SLOT(addToSplitterDataElementFilter(QTreeWidgetItem *)));

    GuiUtils::autoResizeColumnWidths(filterListWidget, 3);

    filterListWidget->update();
}

void InspectorWidget::addSegmentationToSplitter(SegmentationDataWrapper sdw)
{
    segmentationInspectorWidget = new SegmentationInspector(this, vgs_, sdw);

    om::connect(segmentationInspectorWidget->nameEdit, SIGNAL(editingFinished()),
                this, SLOT(nameEditChanged()));

    om::connect(segmentationInspectorWidget->nameEdit, SIGNAL(editingFinished()),
                this, SLOT(nameEditChanged()));

    inspectorProperties->setOrReplaceWidget(segmentationInspectorWidget,
                                            QString("Segmentation %1 Inspector").arg(sdw.getID()));
}

void InspectorWidget::addToSplitterDataElementFilter(QTreeWidgetItem* current)
{
    QVariant result = current->data(USER_DATA_COL, Qt::UserRole);
    FilterDataWrapper fdw = result.value < FilterDataWrapper > ();

    filObjectInspectorWidget = new FilObjectInspector(this, fdw);

    inspectorProperties->setOrReplaceWidget(filObjectInspectorWidget,
                                            QString("Filter %1 Inspector").arg(fdw.getID()));
}

void InspectorWidget::addChannelToVolume()
{
    ChannelDataWrapper cdw;
    cdw.Create();

    populateDataSrcListWidget();
    addChannelToSplitter(cdw);
}

void InspectorWidget::addSegmentationToVolume()
{
    SegmentationDataWrapper sdw;
    sdw.Create();

    populateDataSrcListWidget();

    updateSegmentListBox(sdw);

    addSegmentationToSplitter(sdw);
}

void InspectorWidget::doDataSrcContextMenuVolAdd(QAction* act)
{
    if (act == addChannelAct) {
        addChannelToVolume();
    } else if (act == addSegmentationAct) {
        addSegmentationToVolume();
    } else {
        throw OmFormatException("could not match QAction type...\n");
    }
}

void InspectorWidget::leftClickOnFilterItem(QTreeWidgetItem* current)
{
    if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
        addToSplitterDataSource(current);
    }
}

void InspectorWidget::leftClickOnDataSourceItem(QTreeWidgetItem* current)
{
    if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
        doShowDataSrcContextMenu(current);
    } else {
        addToSplitterDataSource(current);
    }
}

ViewType InspectorWidget::getViewType(QAction* act)
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

void InspectorWidget::addFilter()
{
    ChannelDataWrapper cdw = channelInspectorWidget->getChannelDataWrapper();
    cdw.GetChannel().FilterManager().AddFilter();
    populateDataSrcListWidget();
    populateFilterListWidget(cdw);
}

void InspectorWidget::nameEditChanged()
{
    printf("FIXME: purcaro: sourceEditChangedSeg\n");
    /*
      QVariant result = proxyModel->data(view->currentIndex(), Qt::UserRole);
      int item_type = result.value<int>();

      if(item_type == CHANNEL)
      proxyModel->setData(view->currentIndex(), QVariant(channelInspectorWidget->nameEdit->text()), Qt::EditRole);
      else if(item_type == SEGMENTATION)
      proxyModel->setData(view->currentIndex(), QVariant(segmentationInspectorWidget->nameEdit->text()), Qt::EditRole);
      else if(item_type == SEGMENT)
      proxyModel->setData(view->currentIndex(), QVariant(segmentInspectorWidget->nameEdit->text()), Qt::EditRole);
      else if(FILTER == item_type)
      proxyModel->setData(view->currentIndex(), QVariant(filObjectInspectorWidget->nameEdit->text()), Qt::EditRole);
    */
}

void InspectorWidget::refreshWidgetData()
{
    populateDataSrcListWidget();
    ElementListBox::PopulateLists();
}

void InspectorWidget::rebuildSegmentLists(const OmID segmentationID, const OmSegID segID)
{
    ElementListBox::RebuildLists(SegmentDataWrapper(segmentationID, segID));
}

//////////////////////////////
///////// Data Source Box Stuff
//////////////////////////////
QTreeWidget *InspectorWidget::setupDataSrcList()
{
    dataSrcListWidget = new QTreeWidget(this);
    dataSrcListWidget->setAlternatingRowColors(false);
    dataSrcListWidget->setColumnCount(3);

    dataSrcListWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    QStringList headers;
    headers << tr("enabled") << tr("Name") << tr("ID") << tr("Notes");
    dataSrcListWidget->setHeaderLabels(headers);

    populateDataSrcListWidget();

    // context menu setup
    dataSrcListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    om::connect(dataSrcListWidget, SIGNAL(customContextMenuRequested(const QPoint &)),
                this, SLOT(showDataSrcContextMenu(const QPoint &)));

    return dataSrcListWidget;
}

void InspectorWidget::showDataSrcContextMenu(const QPoint & menuPoint)
{
    QTreeWidgetItem *dataSrcItem = dataSrcListWidget->itemAt(menuPoint);
    if (!dataSrcItem)
    {	// right click occured in "white space" of widget
        om::connect(makeDataSrcContextMenu(dataSrcListWidget),
                    SIGNAL(triggered(QAction *)), this, SLOT(doDataSrcContextMenuVolAdd(QAction *)));

        contextMenuDataSrc->exec(dataSrcListWidget->mapToGlobal(menuPoint));
        return;
    }
    doShowDataSrcContextMenu(dataSrcItem);
}

QMenu *InspectorWidget::makeDataSrcContextMenu(QTreeWidget* parent)
{
    addChannelAct = new QAction(tr("Add Channel"), parent);

    addSegmentationAct = new QAction(tr("Add Segmentation"), parent);

    contextMenuDataSrc = new QMenu(parent);
    contextMenuDataSrc->addAction(addChannelAct);
    contextMenuDataSrc->addAction(addSegmentationAct);

    return contextMenuDataSrc;
}

void InspectorWidget::doShowDataSrcContextMenu(QTreeWidgetItem *dataSrcItem)
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

void InspectorWidget::showChannelContextMenu()
{
    om::connect(makeContextMenuBase(dataSrcListWidget), SIGNAL(triggered(QAction *)),
                this, SLOT(selectChannelView(QAction *)));

    contextMenu->exec(QCursor::pos());
}

void InspectorWidget::showSegmentationContextMenu()
{
    om::connect(makeSegmentationContextMenu(dataSrcListWidget), SIGNAL(triggered(QAction *)),
                this, SLOT(selectSegmentationView(QAction *)));

    contextMenu->exec(QCursor::pos());
}

QMenu *InspectorWidget::makeContextMenuBase(QTreeWidget* parent)
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

QMenu* InspectorWidget::makeSegmentationContextMenu(QTreeWidget* parent)
{
    examMSTAct = new QAction(tr("&Examine MST"), parent);
    examMSTAct->setStatusTip(tr("View the MST table"));

    QMenu* menu = makeContextMenuBase(parent);
    menu->addAction(examMSTAct);

    return menu;
}

void InspectorWidget::selectChannelView(QAction* act)
{
    ChannelDataWrapper cdw = getCurrentlySelectedChannel();
    if(propAct == act){
        addChannelToSplitter(cdw);
    } else if (delAct == act) {
        deleteChannel(cdw);
    } else {
        triggerChannelView(cdw.getID(), getViewType(act));
    }
}

void InspectorWidget::addToSplitterDataSource(QTreeWidgetItem* current)
{
    QVariant result = current->data(USER_DATA_COL, Qt::UserRole);
    DataWrapperContainer dwc = result.value < DataWrapperContainer > ();

    switch (dwc.getType()) {
    case CHANNEL:
        populateFilterListWidget(dwc.getChannelDataWrapper());
        break;
    case SEGMENTATION:
        updateSegmentListBox(dwc.GetSDW());
        break;
    }
}

ChannelDataWrapper InspectorWidget::getCurrentlySelectedChannel()
{
    QTreeWidgetItem *dataSrcItem = dataSrcListWidget->currentItem();
    QVariant result = dataSrcItem->data(USER_DATA_COL, Qt::UserRole);
    DataWrapperContainer dwc = result.value < DataWrapperContainer > ();
    return dwc.getChannelDataWrapper();
}

SegmentationDataWrapper InspectorWidget::getCurrentlySelectedSegmentation()
{
    QTreeWidgetItem *dataSrcItem = dataSrcListWidget->currentItem();
    QVariant result = dataSrcItem->data(USER_DATA_COL, Qt::UserRole);
    DataWrapperContainer dwc = result.value < DataWrapperContainer > ();
    return dwc.GetSDW();
}

void InspectorWidget::selectSegmentationView(QAction* act)
{
    SegmentationDataWrapper sdw = getCurrentlySelectedSegmentation();
    updateSegmentListBox(sdw);

    if(propAct == act){
        addSegmentationToSplitter(sdw);
    } else if (delAct == act) {
        deleteSegmentation(sdw);
    } else if (examMSTAct == act){
        showMSTtable(sdw);
    } else {
        triggerSegmentationView(sdw.getID(), getViewType(act));
    }
}

void InspectorWidget::addChannelToSplitter(ChannelDataWrapper cdw)
{
    channelInspectorWidget = new ChanInspector(cdw, this);

    om::connect(channelInspectorWidget->addFilterButton, SIGNAL(clicked()),
                this, SLOT(addFilter()));

    inspectorProperties->setOrReplaceWidget(channelInspectorWidget,
                                            QString("Channel %1 Inspector").arg(cdw.getID()));
}

void InspectorWidget::deleteSegmentation(SegmentationDataWrapper sdw)
{
    OmAskQuestion confirmDelete;
    confirmDelete.setText("Volume is about to be deleted!!");
    confirmDelete.setInformativeText("Are you certain you want to delete this volume?");

    if(confirmDelete.Ask())
    {
        ElementListBox::Reset();

        const OmID segmentationID = sdw.getID();

        mainWindow_->cleanViewsOnVolumeChange(CHANNEL, segmentationID);

        FOR_EACH(channelID, ChannelDataWrapper::ValidIDs())
        {
            ChannelDataWrapper cdw(*channelID);
            const std::vector<OmFilter2d*> filters = cdw.GetFilters();

            FOR_EACH(iter, filters)
            {
                OmFilter2d* filter = *iter;

                if(om::OVERLAY_SEGMENTATION == filter->FilterType())
                {
                    OmSegmentation* segmentation = filter->GetSegmentation();

                    if(segmentation->getID() == segmentationID){
                        mainWindow_->cleanViewsOnVolumeChange(CHANNEL, *channelID);
                    }
                }
            }
        }

        mainWindow_->cleanViewsOnVolumeChange(SEGMENTATION, segmentationID);

        inspectorProperties->closeDialog();

        sdw.Remove();
        populateDataSrcListWidget();
    }
}

void InspectorWidget::deleteChannel(ChannelDataWrapper cdw)
{
    OmAskQuestion confirmDelete;
    confirmDelete.setText("Volume is about to be deleted!!");
    confirmDelete.setInformativeText("Are you certain you want to delete this volume?");

    if(confirmDelete.Ask())
    {
        inspectorProperties->closeDialog();
        ElementListBox::Reset();
        mainWindow_->cleanViewsOnVolumeChange(CHANNEL, cdw.getID());
        cdw.Remove();
        populateDataSrcListWidget();
    }
}

void InspectorWidget::updateSegmentListBox(SegmentationDataWrapper sdw)
{
    ElementListBox::UpdateSegmentListBox(sdw);
}

void InspectorWidget::showMSTtable(SegmentationDataWrapper sdw)
{
    new MstViewer(this, sdw);
}

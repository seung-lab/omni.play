#include "common/enums.hpp"
#include "common/omDebug.h"
#include "gui/guiUtils.hpp"
#include "gui/inspectors/channel/channelInspector.hpp"
#include "gui/inspectors/filObjectInspector.h"
#include "gui/inspectors/inspectorProperties.h"
#include "gui/inspectors/segmentation/segmentationInspector.hpp"
#include "gui/mainWindow/mainWindow.h"
#include "gui/mstViewer.hpp"
#include "gui/segmentLists/elementListBox.hpp"
#include "gui/sidebars/left/inspectorWidget.h"
#include "gui/viewGroup/viewGroup.h"
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
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);

    verticalLayout->addWidget(setupDataSrcList());

    inspectorProperties_ = new InspectorProperties(this, vgs_);

    ElementListBox::Create(vgs_);
    verticalLayout->addWidget(ElementListBox::Widget());

    channelInspectorWidget_ = NULL;

    QMetaObject::connectSlotsByName(this);

    om::connect(this, SIGNAL(triggerChannelView(om::common::ID, om::common::ViewType)),
                this, SLOT(openChannelView(om::common::ID, om::common::ViewType)));

    om::connect(this, SIGNAL(triggerSegmentationView(om::common::ID, om::common::ViewType)),
                this, SLOT(openSegmentationView(om::common::ID, om::common::ViewType)));

    OmAppState::SetInspector(this);
}

InspectorWidget::~InspectorWidget()
{
    ElementListBox::Delete();
    OmAppState::SetInspector(NULL);
}

void InspectorWidget::openChannelView(om::common::ID chanID, om::common::ViewType vtype)
{
    const ChannelDataWrapper cdw(chanID);
    vgs_->GetViewGroup()->AddView2Dchannel(cdw, vtype);
}

void InspectorWidget::openSegmentationView(om::common::ID segmentationID, om::common::ViewType vtype)
{
    const SegmentationDataWrapper sdw(segmentationID);
    vgs_->GetViewGroup()->AddView2Dsegmentation(sdw, vtype);
}

QTreeWidget* InspectorWidget::setupFilterList()
{
    filterListWidget_ = new QTreeWidget(this);
    filterListWidget_->setAlternatingRowColors(false);
    filterListWidget_->setColumnCount(3);

    QStringList headers;
    headers << tr("enabled") << tr("Name") << tr("ID") << tr("Notes");
    filterListWidget_->setHeaderLabels(headers);

    return filterListWidget_;
}

void InspectorWidget::setRowFlagsAndCheckState(QTreeWidgetItem* row, Qt::CheckState checkState)
{
    row->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    row->setCheckState(ENABLED_COL, checkState);
}

void InspectorWidget::populateDataSrcListWidget()
{
    dataSrcListWidget_->clear();

    const om::common::IDSet& validChanIDs = ChannelDataWrapper::ValidIDs();
    FOR_EACH(iter, validChanIDs)
    {
        const om::common::ID channID = *iter;
        DataWrapperContainer dwc = DataWrapperContainer(om::common::CHANNEL, channID);
        ChannelDataWrapper cdw = dwc.getChannelDataWrapper();
        QTreeWidgetItem *row = new QTreeWidgetItem(dataSrcListWidget_);
        row->setText(NAME_COL, cdw.GetName());
        row->setText(ID_COL, QString("%1").arg(cdw.GetID()));
        row->setText(NOTE_COL, cdw.getNote());
        row->setData(USER_DATA_COL, Qt::UserRole, qVariantFromValue(dwc));
        setRowFlagsAndCheckState(row, GuiUtils::getCheckState(cdw.isEnabled()));
    }

    const om::common::IDSet& validSegIDs = SegmentationDataWrapper::ValidIDs();
    FOR_EACH(iter, validSegIDs)
    {
        const om::common::ID segmenID = *iter;
        DataWrapperContainer dwc = DataWrapperContainer(om::common::SEGMENTATION, segmenID);
        SegmentationDataWrapper sdw = dwc.GetSDW();
        QTreeWidgetItem *row = new QTreeWidgetItem(dataSrcListWidget_);
        row->setText(NAME_COL, sdw.GetName());
        row->setText(ID_COL, QString("%1").arg(sdw.GetID()));
        row->setText(NOTE_COL, sdw.getNote());
        row->setData(USER_DATA_COL, Qt::UserRole, qVariantFromValue(dwc));
        setRowFlagsAndCheckState(row, GuiUtils::getCheckState(sdw.isEnabled()));
    }

    const om::common::IDSet& validAffIDs = AffinityGraphDataWrapper::ValidIDs();
    FOR_EACH(iter, validAffIDs)
    {
        const om::common::ID affID = *iter;
        DataWrapperContainer dwc = DataWrapperContainer(om::common::AFFINITY, affID);
        AffinityGraphDataWrapper adw = dwc.GetADW();
        QTreeWidgetItem *row = new QTreeWidgetItem(dataSrcListWidget_);
        row->setText(NAME_COL, adw.GetName());
        row->setText(ID_COL, QString("%1").arg(adw.GetID()));
        //row->setText(NOTE_COL, adw.getNote());
        row->setData(USER_DATA_COL, Qt::UserRole, qVariantFromValue(dwc));
        setRowFlagsAndCheckState(row, GuiUtils::getCheckState(adw.isEnabled()));
    }

    dataSrcListWidget_->disconnect(SIGNAL(itemClicked(QTreeWidgetItem *, int)));

    std::cout << "dataSrcListWidget_: " << dataSrcListWidget_ << "this: " << this << std::endl;

    om::connect(dataSrcListWidget_, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
                this, SLOT(leftClickOnDataSourceItem(QTreeWidgetItem *)));

    GuiUtils::autoResizeColumnWidths(dataSrcListWidget_, 3);

    dataSrcListWidget_->update();
}

void InspectorWidget::populateFilterListWidget(ChannelDataWrapper cdw)
{
    ElementListBox::Reset();
    ElementListBox::SetTitle(QString("Channel %1").arg(cdw.GetID()));
    ElementListBox::AddTab(0, setupFilterList(), "Filters");

    filterListWidget_->clear();

    filterListWidget_->selectionModel()->blockSignals(true);
    filterListWidget_->selectionModel()->clearSelection();

    std::vector<OmFilter2d*> filters = cdw.GetFilters();

    FOR_EACH(iter, filters)
    {
        OmFilter2d* filterPtr = *iter;
        FilterDataWrapper filter(cdw.GetID(), filterPtr->GetID());
        QTreeWidgetItem *row = new QTreeWidgetItem(filterListWidget_);
        row->setText(NAME_COL, filter.getName());
        row->setText(ID_COL, QString("%1").arg(filter.GetID()));
        row->setData(USER_DATA_COL, Qt::UserRole, qVariantFromValue(filter));
        //row->setText(NOTE_COL, filter->getNote());
        row->setText(NOTE_COL, "");
        setRowFlagsAndCheckState(row, GuiUtils::getCheckState(true));
        //row->setSelected(seg.isSelected());
    }

    filterListWidget_->selectionModel()->blockSignals(false);

    filterListWidget_->disconnect(SIGNAL(itemClicked(QTreeWidgetItem *, int)));
    om::connect(filterListWidget_, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
                this, SLOT(addToSplitterDataElementFilter(QTreeWidgetItem *)));

    GuiUtils::autoResizeColumnWidths(filterListWidget_, 3);

    filterListWidget_->update();
}

void InspectorWidget::addSegmentationToSplitter(SegmentationDataWrapper sdw)
{
    SegmentationInspector* w = new SegmentationInspector(this, inspectorProperties_, sdw);
    inspectorProperties_->SetOrReplaceWidget(w, w->Title());
}

void InspectorWidget::addToSplitterDataElementFilter(QTreeWidgetItem* current)
{
    QVariant result = current->data(USER_DATA_COL, Qt::UserRole);
    FilterDataWrapper fdw = result.value < FilterDataWrapper > ();

    filObjectInspectorWidget_ = new FilObjectInspector(this, fdw);

    inspectorProperties_->SetOrReplaceWidget(filObjectInspectorWidget_,
                                             QString("Filter %1 Inspector").arg(fdw.GetID()));
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

void InspectorWidget::addAffinityToVolume()
{
    AffinityGraphDataWrapper adw;
    adw.Create();

    populateDataSrcListWidget();
}

void InspectorWidget::doDataSrcContextMenuVolAdd(QAction* act)
{
    if (act == addChannelAct_) {
        addChannelToVolume();
    } else if (act == addSegmentationAct_) {
        addSegmentationToVolume();
    } else {
        throw om::FormatException("could not match QAction type...\n");
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

om::common::ViewType InspectorWidget::getViewType(QAction* act)
{
    if (act == xyAct_) {
        return om::common::XY_VIEW;
    } else if (act == xzAct_) {
        return om::common::XZ_VIEW;
    } else if (act == yzAct_) {
        return om::common::ZY_VIEW;
    } else {
        throw om::FormatException("could not match QAction type...\n");
    }
}

void InspectorWidget::nameEditChanged()
{
    printf("FIXME: purcaro: sourceEditChangedSeg\n");
    /*
      QVariant result = proxyModel->data(view->currentIndex(), Qt::UserRole);
      int item_type = result.value<int>();

      if(item_type == om::common::CHANNEL)
      proxyModel->setData(view->currentIndex(), QVariant(channelInspectorWidget_->nameEdit->text()), Qt::EditRole);
      else if(item_type == om::common::SEGMENTATION)
      proxyModel->setData(view->currentIndex(), QVariant(segmentationInspectorWidget->nameEdit->text()), Qt::EditRole);
      else if(item_type == SEGMENT)
      proxyModel->setData(view->currentIndex(), QVariant(segmentInspectorWidget->nameEdit->text()), Qt::EditRole);
      else if(FILTER == item_type)
      proxyModel->setData(view->currentIndex(), QVariant(filObjectInspectorWidget_->nameEdit->text()), Qt::EditRole);
    */
}

void InspectorWidget::refreshWidgetData()
{
    populateDataSrcListWidget();
    ElementListBox::PopulateLists();
}

void InspectorWidget::rebuildSegmentLists(const om::common::ID segmentationID, const om::common::SegID segID)
{
    ElementListBox::RebuildLists(SegmentDataWrapper(segmentationID, segID));
}

//////////////////////////////
///////// Data Source Box Stuff
//////////////////////////////
QTreeWidget *InspectorWidget::setupDataSrcList()
{
    dataSrcListWidget_ = new QTreeWidget(this);
    dataSrcListWidget_->setAlternatingRowColors(false);
    dataSrcListWidget_->setColumnCount(3);

    dataSrcListWidget_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    QStringList headers;
    headers << tr("enabled") << tr("Name") << tr("ID") << tr("Notes");
    dataSrcListWidget_->setHeaderLabels(headers);

    populateDataSrcListWidget();

    // context menu setup
    dataSrcListWidget_->setContextMenuPolicy(Qt::CustomContextMenu);
    om::connect(dataSrcListWidget_, SIGNAL(customContextMenuRequested(const QPoint &)),
                this, SLOT(showDataSrcContextMenu(const QPoint &)));

    return dataSrcListWidget_;
}

void InspectorWidget::showDataSrcContextMenu(const QPoint & menuPoint)
{
    QTreeWidgetItem *dataSrcItem = dataSrcListWidget_->itemAt(menuPoint);

    if(dataSrcItem)
    {
        doShowDataSrcContextMenu(dataSrcItem);
        return;
    }

    // right click occured in "white space" of widget
    om::connect(makeDataSrcContextMenu(dataSrcListWidget_), SIGNAL(triggered(QAction *)),
                this, SLOT(doDataSrcContextMenuVolAdd(QAction *)));

    contextMenuDataSrc_->exec(dataSrcListWidget_->mapToGlobal(menuPoint));
}

QMenu *InspectorWidget::makeDataSrcContextMenu(QTreeWidget* parent)
{
    addChannelAct_ = new QAction(tr("Add Channel"), parent);

    addSegmentationAct_ = new QAction(tr("Add Segmentation"), parent);

    //addAffinityAct_ = new QAction(tr("Add Affinity"), parent);

    contextMenuDataSrc_ = new QMenu(parent);
    contextMenuDataSrc_->addAction(addChannelAct_);
    contextMenuDataSrc_->addAction(addSegmentationAct_);
    //contextMenuDataSrc_->addAction(addAffinityAct_);

    return contextMenuDataSrc_;
}

void InspectorWidget::doShowDataSrcContextMenu(QTreeWidgetItem *dataSrcItem)
{
    QVariant result = dataSrcItem->data(USER_DATA_COL, Qt::UserRole);
    DataWrapperContainer dwc = result.value<DataWrapperContainer>();

    switch (dwc.getType()) {
    case om::common::CHANNEL:
        showChannelContextMenu();
        break;

    case om::common::SEGMENTATION:
        showSegmentationContextMenu();
        break;

    case om::common::AFFINITY:
        showAffinityContextMenu();
    }
}

void InspectorWidget::showChannelContextMenu()
{
    om::connect(makeContextMenuBase(dataSrcListWidget_), SIGNAL(triggered(QAction *)),
                this, SLOT(selectChannelView(QAction *)));

    contextMenu_->exec(QCursor::pos());
}

void InspectorWidget::showSegmentationContextMenu()
{
    om::connect(makeSegmentationContextMenu(dataSrcListWidget_), SIGNAL(triggered(QAction *)),
                this, SLOT(selectSegmentationView(QAction *)));

    contextMenu_->exec(QCursor::pos());
}

void InspectorWidget::showAffinityContextMenu()
{
}

QMenu *InspectorWidget::makeContextMenuBase(QTreeWidget* parent)
{
    xyAct_ = new QAction(tr("&View XY"), parent);
    xyAct_->setStatusTip(tr("Opens the XY view"));

    xzAct_ = new QAction(tr("&View XZ"), parent);
    xzAct_->setStatusTip(tr("Opens the XZ view"));

    yzAct_ = new QAction(tr("&View YZ"), parent);
    yzAct_->setStatusTip(tr("Opens the YZ view"));

    propAct_ = new QAction(tr("&Properties"), parent);
    propAct_->setStatusTip(tr("Opens properties"));

    delAct_ = new QAction(tr("&Delete"), parent);
    propAct_->setStatusTip(tr("Deletes a Volume"));

    contextMenu_ = new QMenu(parent);
    contextMenu_->addAction(xyAct_);
    contextMenu_->addAction(xzAct_);
    contextMenu_->addAction(yzAct_);
    contextMenu_->addAction(propAct_);
    contextMenu_->addAction(delAct_);

    return contextMenu_;
}

QMenu* InspectorWidget::makeSegmentationContextMenu(QTreeWidget* parent)
{
    examMSTAct_ = new QAction(tr("&Examine MST"), parent);
    examMSTAct_->setStatusTip(tr("View the MST table"));

    QMenu* menu = makeContextMenuBase(parent);
    menu->addAction(examMSTAct_);

    return menu;
}

void InspectorWidget::selectChannelView(QAction* act)
{
    ChannelDataWrapper cdw = getCurrentlySelectedChannel();

    if(propAct_ == act){
        addChannelToSplitter(cdw);

    } else if (delAct_ == act) {
        deleteChannel(cdw);

    } else {
        triggerChannelView(cdw.GetID(), getViewType(act));
    }
}

void InspectorWidget::addToSplitterDataSource(QTreeWidgetItem* current)
{
    QVariant result = current->data(USER_DATA_COL, Qt::UserRole);
    DataWrapperContainer dwc = result.value < DataWrapperContainer > ();

    switch (dwc.getType()) {
    case om::common::CHANNEL:
        populateFilterListWidget(dwc.getChannelDataWrapper());
        break;

    case om::common::SEGMENTATION:
        updateSegmentListBox(dwc.GetSDW());
        break;

    case om::common::AFFINITY:
        // TODO: something
        break;
    }
}

ChannelDataWrapper InspectorWidget::getCurrentlySelectedChannel()
{
    QTreeWidgetItem *dataSrcItem = dataSrcListWidget_->currentItem();
    QVariant result = dataSrcItem->data(USER_DATA_COL, Qt::UserRole);
    DataWrapperContainer dwc = result.value < DataWrapperContainer > ();
    return dwc.getChannelDataWrapper();
}

SegmentationDataWrapper InspectorWidget::getCurrentlySelectedSegmentation()
{
    QTreeWidgetItem *dataSrcItem = dataSrcListWidget_->currentItem();
    QVariant result = dataSrcItem->data(USER_DATA_COL, Qt::UserRole);
    DataWrapperContainer dwc = result.value < DataWrapperContainer > ();
    return dwc.GetSDW();
}

void InspectorWidget::selectSegmentationView(QAction* act)
{
    SegmentationDataWrapper sdw = getCurrentlySelectedSegmentation();
    updateSegmentListBox(sdw);

    if(propAct_ == act){
        addSegmentationToSplitter(sdw);

    } else if (delAct_ == act) {
        deleteSegmentation(sdw);

    } else if (examMSTAct_ == act){
        showMSTtable(sdw);

    } else {
        triggerSegmentationView(sdw.GetID(), getViewType(act));
    }
}

void InspectorWidget::addChannelToSplitter(ChannelDataWrapper cdw)
{
    channelInspectorWidget_ = new ChannelInspector(this, inspectorProperties_, cdw);

    inspectorProperties_->SetOrReplaceWidget(channelInspectorWidget_,
                                             channelInspectorWidget_->Title());
}

void InspectorWidget::deleteSegmentation(SegmentationDataWrapper sdw)
{
    OmAskQuestion confirmDelete;
    confirmDelete.setText("Volume is about to be deleted!!");
    confirmDelete.setInformativeText("Are you certain you want to delete this volume?");

    if(!confirmDelete.Ask()){
        return;
    }

    ElementListBox::Reset();

    const om::common::ID segmentationID = sdw.GetID();

    mainWindow_->cleanViewsOnVolumeChange(om::common::CHANNEL, segmentationID);

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

                if(segmentation->GetID() == segmentationID){
                    mainWindow_->cleanViewsOnVolumeChange(om::common::CHANNEL, *channelID);
                }
            }
        }
    }

    mainWindow_->cleanViewsOnVolumeChange(om::common::SEGMENTATION, segmentationID);

    inspectorProperties_->CloseDialog();

    sdw.Remove();
    populateDataSrcListWidget();
}

void InspectorWidget::deleteChannel(ChannelDataWrapper cdw)
{
    OmAskQuestion confirmDelete;
    confirmDelete.setText("Volume is about to be deleted!!");
    confirmDelete.setInformativeText("Are you certain you want to delete this volume?");

    if(confirmDelete.Ask())
    {
        inspectorProperties_->CloseDialog();
        ElementListBox::Reset();
        mainWindow_->cleanViewsOnVolumeChange(om::common::CHANNEL, cdw.GetID());
        cdw.Remove();
        populateDataSrcListWidget();
    }
}

void InspectorWidget::updateSegmentListBox(SegmentationDataWrapper sdw){
    ElementListBox::UpdateSegmentListBox(sdw);
}

void InspectorWidget::showMSTtable(SegmentationDataWrapper sdw){
    new MstViewer(this, sdw);
}

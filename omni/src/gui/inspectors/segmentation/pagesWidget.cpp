#include "gui/inspectors/segmentation/pagesWidget.h"
#include "gui/inspectors/segmentation/buildPage/pageBuilder.hpp"
#include "gui/inspectors/segmentation/exportPage/pageExport.h"
#include "gui/inspectors/segmentation/metadata/metadataPage.hpp"

om::segmentationInspector::PagesWidget::PagesWidget(QWidget* parent,
                                                    const SegmentationDataWrapper& sdw)
    : QStackedWidget(parent)
{
    addWidget(new PageBuilder(this, sdw));
    addWidget(new PageMetadata(this, sdw));
    addWidget(new PageExport(this, sdw));
}

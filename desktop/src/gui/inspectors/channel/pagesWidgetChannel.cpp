#include "gui/inspectors/channel/pagesWidgetChannel.h"
#include "gui/inspectors/channel/buildPage/pageBuilderChannel.hpp"
#include "gui/inspectors/channel/metadata/metadataPageChannel.hpp"
#include "gui/inspectors/channel/exportPage/pageExportChannel.h"

om::channelInspector::PagesWidget::PagesWidget(QWidget* parent,
                                               const ChannelDataWrapper& cdw)
    : QStackedWidget(parent) {
  addWidget(new PageBuilder(this, cdw));
  addWidget(new PageMetadata(this, cdw));
  addWidget(new PageExport(this, cdw));
}

#include "inspectorProperties.h"
#include "viewGroup/omViewGroupState.h"
#include "gui/updateSegmentProperties.h"

InspectorProperties::InspectorProperties(QWidget* parent,
                                         OmViewGroupState* vgs)
    : QDialog(parent)
    , vgs_(vgs)
    , widget_(NULL)
{
    mainLayout = new QVBoxLayout();
    setLayout(mainLayout);

    vgs_->SetInspectorProperties(this);

    UpdateSegmentPropertiesDialog::SetInspectorProperties(this);
}

InspectorProperties::~InspectorProperties()
{
    UpdateSegmentPropertiesDialog::Delete();
}

void InspectorProperties::setOrReplaceWidget(QWidget *incomingWidget,
                                             const QString title)
{
    if(widget_)
    {
        mainLayout->removeWidget( widget_ );
        widget_->close();
        delete( widget_ );
        widget_ = NULL;
    }

    widget_ = incomingWidget;
    mainLayout->addWidget(widget_);
    setWindowTitle(title);

    if(!isVisible())
    {
        show();
        raise();
        activateWindow();
    }
}

void InspectorProperties::closeDialog(){
    QDialog::done(0);
}

OmViewGroupState* InspectorProperties::getViewGroupState(){
    return vgs_;
}

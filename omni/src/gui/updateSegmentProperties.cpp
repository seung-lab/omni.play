#include "gui/updateSegmentProperties.h"
#include "gui/updateSegmentPropertiesImpl.hpp"

UpdateSegmentPropertiesDialog::UpdateSegmentPropertiesDialog()
{}

UpdateSegmentPropertiesDialog::~UpdateSegmentPropertiesDialog()
{}

void UpdateSegmentPropertiesDialog::SetInspectorProperties(InspectorProperties* ip){
    instance().impl_.reset(new UpdateSegmentPropertiesDialogImpl(ip));
}

void UpdateSegmentPropertiesDialog::Delete(){
    instance().impl_.reset();
}

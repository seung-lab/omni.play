#include "gui/sidebars/right/rightImpl.h"
#include "gui/sidebars/right/annotationGroup/annotationGroup.h"
#include "gui/sidebars/right/annotationGroup/annotationListWidget.hpp"
#include "system/omConnect.hpp"
#include "gui/widgets/omButton.hpp"
#include "viewGroup/omViewGroupState.h"
#include "events/omEvents.h"

namespace om {
namespace sidebars {

class ToggleAnnotationButton : public OmButton<QWidget>
{
public:
    ToggleAnnotationButton(QWidget *widget, OmViewGroupState *vgs)
    : OmButton< QWidget >(widget,
                          "show annotations",
                          "show annotations",
                          true)
    , vgs_(vgs)
    { }

private:
    OmViewGroupState *vgs_;

    void doAction() {
        vgs_->setAnnotationVisible(!this->isChecked());
        OmEvents::Redraw2d();
        OmEvents::Redraw3d();
    }
};


AnnotationGroup::AnnotationGroup(om::sidebars::rightImpl* parent, OmViewGroupState* vgs)
    : OmWidget(parent)
    , vgs_(vgs)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    ToggleAnnotationButton *toggleAnnotationsButton =
        new ToggleAnnotationButton(this, vgs_);

    toggleAnnotationsButton->setChecked(vgs_->getAnnotationVisible());
    layout->addWidget(toggleAnnotationsButton);
    layout->addWidget(new AnnotationListWidget(this, vgs_));
    this->setLayout(layout);
}

} // namespace sidebars
} // namespace om

#include "gui/annotationToolbox/annotationToolbox.h"

namespace om {
namespace gui {

AnnotationToolbox::AnnotationToolbox(QWidget* parent, OmViewGroupState* vgs)
    : QDialog(parent, Qt::Tool)
{
    setAttribute(Qt::WA_ShowWithoutActivating);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(new ColorButton(this, vgs));
    layout->addWidget(new AnnotationLineEdit(this, vgs));

    setLayout(layout);
}


} // namespace gui
} // namespace om
#include "users/omGuiUserChooser.h"
#include "users/omGuiUserChooserWidget.h"

#include <QVBoxLayout>

OmGuiUserChooser::OmGuiUserChooser(QWidget *const parent)
    : QDialog(parent, Qt::Tool)
{
    QVBoxLayout* box = new QVBoxLayout();
    box->addWidget(new om::gui::userChooserWidget(this));
    setLayout(box);
}

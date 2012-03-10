#include "landmarks/omLandmarksDialog.h"
#include "landmarks/omLandmarksWidget.h"
#include "landmarks/omLandmarks.hpp"

om::landmarks::dialog::dialog(QWidget *const parent, OmLandmarks* landmarks)
    : QDialog(parent, Qt::Tool)
    , landmarks_(landmarks)
    , widget_(NULL)
    , mainLayout_(new QVBoxLayout())
{
    setLayout(mainLayout_);
}

void om::landmarks::dialog::Reset(const std::vector<sdwAndPt>& pts)
{
    if(widget_)
    {
        mainLayout_->removeWidget(widget_);
        widget_->deleteLater();
    }

    widget_ = new widget(this, pts);

    mainLayout_->addWidget(widget_);
}

void om::landmarks::dialog::ClearPtsAndHide()
{
    landmarks_->ClearPts();
    hide();
}

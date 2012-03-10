#include "landmarks/omLandmarksDialog.h"
#include "landmarks/omLandmarksWidget.h"
#include "landmarks/omLandmarksWidgetButtons.hpp"

om::landmarks::widget::widget(dialog *const dialog,
                              const std::vector<sdwAndPt>& pts)
    : QWidget(dialog)
    , dialog_(dialog)
    , pts_(pts)
{
    create();
}

void om::landmarks::widget::Hide(){
    dialog_->hide();
}

void om::landmarks::widget::create()
{
    QVBoxLayout* box = new QVBoxLayout;

    box->addWidget(new QLabel(showSegmentList(), this));

    if(2 == pts_.size()){
        box->addWidget(new LongRangeConnectionButton(this, dialog_, pts_));
    }

    if(2 <= pts_.size()){
        box->addWidget(new JoinButton(this, dialog_, pts_));
    }

    box->addWidget(new ClearButton(this, dialog_));

    setLayout(box);
}

QString om::landmarks::widget::showSegmentList()
{
    QString s;

    FOR_EACH(iter, pts_){
        s += QString::fromStdString(outputPt(*iter));
    }

    return s;
}

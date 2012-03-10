#include "gui/inspectors/segmentation/notesPage/pageNotes.h"
#include "utility/omStringHelpers.h"

om::segmentationInspector::PageNotes::PageNotes(QWidget* parent, const SegmentationDataWrapper& sdw)
    : QWidget(parent)
    , sdw_(sdw)
{
    QVBoxLayout* overallContainer = new QVBoxLayout(this);
    overallContainer->addWidget(makeNotesBox());
    overallContainer->addStretch(1);
}


QGroupBox* om::segmentationInspector::PageNotes::makeNotesBox()
{
    QGroupBox* notesBox = new QGroupBox("Notes");
    QGridLayout *gridNotes = new QGridLayout( notesBox );

    notesEdit_ = new QPlainTextEdit(notesBox);
    notesEdit_->setObjectName(QString::fromUtf8("notesEdit_"));
    gridNotes->addWidget(notesEdit_, 0, 1);

    return notesBox;
}
#pragma once

#include "utility/segmentationDataWrapper.hpp"
#include <QtGui>

namespace om {
namespace segmentationInspector {

class PageNotes : public QWidget {
private:
    const SegmentationDataWrapper sdw_;

public:
    PageNotes(QWidget* parent, const SegmentationDataWrapper& sdw);

    const SegmentationDataWrapper& GetSDW() const {
        return sdw_;
    }

private:
    QPlainTextEdit* notesEdit_;
    QGroupBox* makeNotesBox();
};

} // namespace segmentationInspector
} // namespace om


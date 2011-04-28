#ifndef PAGE_EXPORTER_HPP
#define PAGE_EXPORTER_HPP

#include "utility/segmentationDataWrapper.hpp"

#include <QtGui>

namespace om {
namespace segmentationInspector {

class PageExport : public QWidget {
private:
    const SegmentationDataWrapper sdw_;

public:
    PageExport(QWidget* parent, const SegmentationDataWrapper& sdw);

    const SegmentationDataWrapper& GetSDW() const {
        return sdw_;
    }

private:
    QPlainTextEdit* notesEdit_;

    QGroupBox* makeActionsBox();
    QGroupBox* makeStatsBox();
    QGroupBox* makeNotesBox();
    QGroupBox* makeToolsBox();
};

} // namespace segmentationInspector
} // namespace om

#endif

#pragma once

#include "landmarks/omLandmarksTypes.h"

#include <QDialog>
#include <QVBoxLayout>

class OmLandmarks;

namespace om {
namespace landmarks {

class widget;

class dialog : public QDialog {
private:
    OmLandmarks *const landmarks_;
    widget* widget_;

    QVBoxLayout* mainLayout_;

public:
    dialog(QWidget *const parent, OmLandmarks* landmarks);

    void Reset(const std::vector<sdwAndPt>& pts);

    void ClearPtsAndHide();
};

} // namespace landmarks
} // namespace om

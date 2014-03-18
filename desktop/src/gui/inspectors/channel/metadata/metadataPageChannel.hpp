#pragma once
#include "precomp.h"

#include "gui/inspectors/volInspector.h"
#include "utility/channelDataWrapper.hpp"
#include "gui/inspectors/absCoordBox.hpp"

namespace om {
namespace channelInspector {

class PageMetadata : public QWidget {
  Q_OBJECT;

 private:
  const ChannelDataWrapper cdw_;

 public:
  PageMetadata(QWidget* parent, const ChannelDataWrapper& cdw)
      : QWidget(parent), cdw_(cdw) {
    QVBoxLayout* overallContainer = new QVBoxLayout(this);
    overallContainer->addWidget(makeVolBox());
    overallContainer->addWidget(makeAbsCoordBox());
    overallContainer->addStretch(1);
  }

 private:
  QGroupBox* makeVolBox() {
    return new OmVolInspector(cdw_.GetChannel(), this);
  }

  QGroupBox* makeAbsCoordBox() {
    return new om::volumeInspector::AbsCoordBox(cdw_.GetChannel(), this);
  }
};

}  // namespace channelInspector
}  // namespace om

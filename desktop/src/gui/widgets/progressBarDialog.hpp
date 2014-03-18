#pragma once
#include "precomp.h"

#include "gui/widgets/omTellInfo.hpp"
#include "gui/widgets/progress.hpp"
#include "gui/widgets/progressBar.hpp"
#include "system/omConnect.hpp"
#include "threads/omTaskManager.hpp"
#include "threads/omTaskManagerTypes.h"

namespace om {
namespace gui {

class progressBarDialog : public QDialog {
  Q_OBJECT;

 private:
  std::shared_ptr<progress> progress_;
  progressBar<uint64_t>* progressBar_;

  OmThreadPool threadPool_;

  void init() {
    threadPool_.start(1);

    progressBar_ = new progressBar<uint64_t>(this);

    QVBoxLayout* box = new QVBoxLayout(this);
    box->addWidget(progressBar_);

    om::connect(progress_.get(), SIGNAL(progressChanged(uint32_t, uint32_t)),
                this, SLOT(update(uint32_t, uint32_t)));

    om::connect(this, SIGNAL(signalTellDone(QString)), this,
                SLOT(tellDone(QString)));

    show();
    raise();
    activateWindow();
  }

 public:
  progressBarDialog(QWidget* parent)
      : QDialog(parent), progress_(std::make_shared<progress>()) {
    init();
  }

  progressBarDialog(QWidget* parent, std::shared_ptr<progress> progress)
      : QDialog(parent), progress_(progress) {
    init();
  }

  ~progressBarDialog() {}

  std::shared_ptr<progress> Progress() { return progress_; }

  void SetTotal(const uint32_t total) { progress_->SetTotal(total); }

  void SetDone(const uint32_t numDid) { progress_->SetDone(numDid); }

  template <typename T>
  void push_back(const T& task) {
    threadPool_.push_back(task);
  }

  void TellDone(const QString str) { signalTellDone(str); }

Q_SIGNALS:
  void signalTellDone(const QString str);

 private
Q_SLOTS:
  void update(const uint32_t numDone, const uint32_t numTotal) {
    progressBar_->SetMaximum(numTotal);
    progressBar_->SetValue(numDone);
  }

  void tellDone(const QString str) {
    hide();
    OmTellInfo tell(str);
    deleteLater();
  }
};

}  // namespace gui
}  // namespace om

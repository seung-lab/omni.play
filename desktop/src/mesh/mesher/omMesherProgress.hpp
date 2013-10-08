#pragma once

#include "common/omCommon.h"
#include "gui/widgets/progress.hpp"

namespace om {
namespace mesher {

class progress {
 private:
  om::shared_ptr<om::gui::progress> progress_;

 public:
  progress() : progress_(om::make_shared<om::gui::progress>()) {}

  om::shared_ptr<om::gui::progress> Progress() { return progress_; }

  void Progress(om::shared_ptr<om::gui::progress> p) { progress_ = p; }

  void SetTotalNumChunks(const uint32_t totalNumChunks) {
    progress_->SetTotal(totalNumChunks);
  }

  uint32_t GetTotalNumChunks() const { return progress_->GetTotal(); }

  void ChunkCompleted(const om::chunkCoord& coord) {
    progress_->SetDone(1);

    const uint32_t total = progress_->GetTotal();
    const uint32_t chunksLeft = total - progress_->GetDone();

    std::cout << "finished chunk: " << coord << "; " << chunksLeft
              << " chunks left "
              << "(" << total << " total)" << std::endl;

  }
};

}  // namespace mesher
}  // namespace om

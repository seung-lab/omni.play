#pragma once

#include "common/common.h"
#include "gui/widgets/progress.hpp"

namespace om {
namespace mesher {

class progress {
private:
    std::shared_ptr<om::gui::progress> progress_;

public:
    progress()
        : progress_(std::make_shared<om::gui::progress>())
    {}

    std::shared_ptr<om::gui::progress> Progress(){
        return progress_;
    }

    void Progress(std::shared_ptr<om::gui::progress> p){
        progress_ = p;
    }

    void SetTotalNumChunks(const uint32_t totalNumChunks){
        progress_->SetTotal(totalNumChunks);
    }

    uint32_t GetTotalNumChunks() const {
        return progress_->GetTotal();
    }

    void ChunkCompleted(const om::chunkCoord& coord)
    {
        progress_->SetDone(1);

        const uint32_t total = progress_->GetTotal();
        const uint32_t chunksLeft = total - progress_->GetDone();

        std::cout << "finished chunk: " << coord << "; "
                  << chunksLeft << " chunks left "
                  << "(" << total << " total)" << std::endl;

    }
};

} // namespace mesher
} // namespace om

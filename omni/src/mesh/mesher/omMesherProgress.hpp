#pragma once

#include "chunks/omChunkCoord.h"
#include "utility/omLockedPODs.hpp"

#include <QObject>

namespace om {
namespace mesher {

class progress : public QObject {
Q_OBJECT

private:
    LockedUint32 totalNumChunks_;
    LockedUint32 numChunksDone_;

public:
    progress()
    {
        totalNumChunks_.set(0);
        numChunksDone_.set(0);
    }

    void SetTotalNumChunks(const uint32_t totalNumChunks){
        totalNumChunks_.set(totalNumChunks);
    }

    uint32_t GetTotalNumChunks() const {
        return totalNumChunks_.get();
    }

    void ChunkCompleted(const OmChunkCoord& coord)
    {
        ++numChunksDone_;
        chunkDone(numChunksDone_.get());

        const uint32_t chunksLeft = totalNumChunks_.get() - numChunksDone_.get();

        std::cout << "finished chunk: " << coord << "; "
                  << chunksLeft << " chunks left "
                  << "(" << totalNumChunks_.get() << " total)\n";

    }

Q_SIGNALS:
    void chunkDone(const uint32_t numDone);
};

} // namespace mesher
} // namespace om

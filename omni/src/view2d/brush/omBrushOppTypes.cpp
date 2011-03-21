#include "view2d/brush/omBrushOppInfo.hpp"

std::ostream& operator<<(std::ostream &out, const OmBrushOppInfo& i)
{
    out << i.segmentation->GetName() << ", "
        << i.viewType << ", "
        << "brushDia: " << i.brushDia << ", "
        << "depth: " << i.depth << ", "
        << "ptsInCircle: " << i.ptsInCircle.size() << ", "
        << "chunkDim: " << i.chunkDim << ", "
        << "AddOrSubtract: " << i.addOrSubract;

    return out;
}


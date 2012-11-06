#include "mesh/drawer/omMeshDrawer.h"
#include "mesh/drawer/omMeshDrawerImpl.hpp"
#include "mesh/drawer/omMeshPlanCache.hpp"
#include "mesh/io/omMeshMetadata.hpp"
#include "mesh/omMeshManager.h"

OmMeshDrawer::OmMeshDrawer(OmSegmentation* segmentation)
    : segmentation_(segmentation)
    , rootSegLists_(boost::make_shared<OmMeshSegmentList>(segmentation))
    , cache_(boost::make_shared<OmMeshPlanCache>(segmentation_,
                                                 rootSegLists_.get()))
    , numPrevRedraws_(0)
{}

boost::optional<std::pair<float,float> >
OmMeshDrawer::Draw(OmViewGroupState* vgs,
                   boost::shared_ptr<OmVolumeCuller> culler,
                   const om::common::Bitfield drawOptions)
{
    if(!segmentation_->MeshManager(1)->Metadata()->IsBuilt()){
        // printf("no meshes found\n");
        return boost::optional<std::pair<float,float> >();
    }

    boost::shared_ptr<OmMeshPlan> sortedSegments =
        cache_->GetSegmentsToDraw(vgs, culler, drawOptions);

    // FOR_EACH(iter, *sortedSegments) {
    // 	std::cout << iter->first->value() << ":" << iter->second << " ";
    // }
    // std::cout << std::endl;

    updateNumPrevRedraws(culler);

    OmMeshDrawerImpl drawer(segmentation_, vgs, drawOptions,
                            sortedSegments.get());

    drawer.Draw(getAllowedDrawTime());

    if(drawer.RedrawNeeded())
    {
        std::pair<float,float> p(drawer.NumVoxelsDrawn(),
                                 drawer.TotalVoxelsPresent());
        return  boost::optional<std::pair<float,float> >(p);
    }else {
        return boost::optional<std::pair<float,float> >();
    }
}

void OmMeshDrawer::updateNumPrevRedraws(boost::shared_ptr<OmVolumeCuller> culler)
{
    if(!culler_ || !culler_->equals(culler)){
        culler_ = culler;
        numPrevRedraws_ = 0;
        return;
    }

    ++numPrevRedraws_;
}

int OmMeshDrawer::getAllowedDrawTime()
{
    static const int maxElapsedDrawTimeMS = 50; // attempt 20 fps...
    static const int maxAllowedDrawTime = 250;
    static const int numRoundsBeforeUpMaxTime = 10;

    if(numPrevRedraws_ < numRoundsBeforeUpMaxTime){
        return maxElapsedDrawTimeMS;
    }

    const int maxRedrawMS =
        (numPrevRedraws_ / numRoundsBeforeUpMaxTime) * maxElapsedDrawTimeMS;
    if(maxRedrawMS > maxAllowedDrawTime){
        return maxAllowedDrawTime;
    }

    return maxRedrawMS;
}

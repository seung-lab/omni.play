#ifndef OM_FIND_CHUNKS_TO_DRAW_HPP
#define OM_FIND_CHUNKS_TO_DRAW_HPP

#include "chunks/omSegChunk.h"
#include "volume/omSegmentation.h"

class OmFindChunksToDraw {
private:
	OmSegmentation *const segmentation_;
	OmVolumeCuller *const culler_;

	boost::shared_ptr<std::list<OmSegChunkPtr> > chunksToDraw_;

public:
	OmFindChunksToDraw(OmSegmentation* segmentation,
					   OmVolumeCuller* culler)
		: segmentation_(segmentation)
		, culler_(culler)
	{}

	boost::shared_ptr<std::list<OmSegChunkPtr> > FindChunksToDraw()
	{
		chunksToDraw_ =  boost::make_shared<std::list<OmSegChunkPtr> >();
		determineChunksToDraw(segmentation_->Coords().RootMipChunkCoordinate(),
							  true);
		return chunksToDraw_;
	}

private:

	/**
	 * Recursively draw MipChunks within the Segmentation using the
	 *  MipCoordinate hierarchy. Uses the OmVolumeCuller to determine
	 *  the visibility of a MipChunk.  If visible, the MipChunk is either
	 *  drawn or the recursive draw process is called on its children.
	 */
	void determineChunksToDraw(const OmChunkCoord& chunkCoord,
							   bool testVis)
	{
		OmSegChunkPtr chunk;
		segmentation_->GetChunk(chunk, chunkCoord);

		if(testVis) {
			//check if frustum contains chunk
			switch (culler_->TestChunk(chunk->Mipping().GetNormExtent())) {
			case VISIBILITY_NONE:
				return;

			case VISIBILITY_PARTIAL:
				// continue drawing tree and continue testing children
				break;

			case VISIBILITY_FULL:
				// continue drawing tree, but assume children are visible
				testVis = false;
				break;
			}
		}

		if( shouldChunkBeDrawn(chunk) ){
			chunksToDraw_->push_back(chunk);

		} else {
			FOR_EACH(iter, chunk->Mipping().GetChildrenCoordinates()){
				determineChunksToDraw(*iter, testVis);
			}
		}
	}

	/**
	 *	Given that the chunk is visible, determine if it should be drawn
	 *	or if we should continue refining so as to draw children.
	 */
	bool shouldChunkBeDrawn(OmSegChunkPtr chunk)
	{
		// draw if MIP 0
		if(0 == chunk->GetCoordinate().Level){
			return true;
		}

		const NormBbox& normExtent = chunk->Mipping().GetNormExtent();
		const NormBbox& clippedNormExtent = chunk->Mipping().GetClippedNormExtent();

		const NormCoord camera = culler_->GetPosition();
		const NormCoord center = clippedNormExtent.getCenter();

		const float camera_to_center = center.distance(camera);
		const float distance =
			(normExtent.getMax() - normExtent.getCenter()).length();

		//if distance too large, just draw it - else keep breaking it down
		return (camera_to_center > distance);
	}
};

#endif

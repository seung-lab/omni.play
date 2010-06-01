#include "mesh/omMeshDrawer.h"
#include "common/omGl.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentIterator.h"
#include "volume/omMipChunk.h"
#include "volume/omVolumeCuller.h"

OmMeshDrawer::OmMeshDrawer(OmSegmentation * seg)
	: mSeg(seg)
	, mSegmentCache(seg->mSegmentCache)
{
}

/////////////////////////////////
///////          Draw

/*
 *	Draw the entire Segmentation.  Starts the recursive processes of drawing MipChunks
 *	from the root MipChunk of the Segmentation.  Filters for relevant data values to be 
 *	drawn depending on culler draw options and passes relevant set to root chunk.
 */
void OmMeshDrawer::Draw(OmVolumeCuller & rCuller, OmViewGroupState * vgs)
{
	mViewGroupState = vgs;		// This is hackish because i don;t want to thread this into all the
					// recursive function calls.
	//transform to normal frame
	glPushMatrix();
	glMultMatrixf(mSeg->mNormToSpaceMat.ml);

	//draw volume axis
	if (rCuller.CheckDrawOption(DRAWOP_DRAW_VOLUME_AXIS)) {
		glDrawPositiveAxis();
	}

	//return if no chunk level drawing
	if (!rCuller.CheckDrawOption(DRAWOP_LEVEL_CHUNKS)) {
		glPopMatrix();
		return;
	}
	//form culler for this volume and call draw on all volumes
	OmVolumeCuller volume_culler =
		rCuller.GetTransformedCuller(mSeg->mNormToSpaceMat, mSeg->mNormToSpaceInvMat);

	OmSegmentIterator iter(mSegmentCache);

	//check to filter for relevant data values
	if (rCuller.CheckDrawOption(DRAWOP_SEGMENT_FILTER_SELECTED)) {
		iter.iterOverSelectedIDs();
		
	} else if (rCuller.CheckDrawOption(DRAWOP_SEGMENT_FILTER_UNSELECTED)) {
		iter.iterOverEnabledIDs();
	}

	if( iter.empty() ){
        	glPopMatrix();
		return;
	}

	glPushName(mSeg->GetId());

	//draw relevant data values starting from root chunk
	DrawChunkRecursive(mSeg->RootMipChunkCoordinate(), iter, true, volume_culler );

	glPopName();

        //pop matrix
        glPopMatrix();

}



/*
 *	Recursively draw MipChunks within the Segmentation using the MipCoordinate hierarchy.
 *	Uses the OmVolumeCuller to determine the visibility of a MipChunk.  If visible, the
 *	MipChunk is either drawn or the recursive draw process is called on its children.
 */
void OmMeshDrawer::DrawChunkRecursive(const OmMipChunkCoord & chunkCoord, 
					OmSegmentIterator segIter,
					bool testVis, 
					OmVolumeCuller & rCuller )
{
	// get pointer to chunk
	QExplicitlySharedDataPointer < OmMipChunk > p_chunk = QExplicitlySharedDataPointer < OmMipChunk > ();
	mSeg->GetChunk(p_chunk, chunkCoord);

	// TEST FOR CHUNK VISIBILITY IF NECESSARY
	if (testVis) {
		//check if frustum contains chunk
		switch (rCuller.TestChunk(*p_chunk)) {
		case VISIBILITY_NONE:
			return;

		case VISIBILITY_PARTIAL:  //continue drawing tree and continue testing children
			break;

		case VISIBILITY_FULL:	  //continue drawing tree, but assume children are visible
			testVis = false;
			break;
		}
	}
	
	// TEST IF CHUNK SHOULD BE DRAWN
	// if chunk satisfies draw criteria
	if ( p_chunk->DrawCheck(rCuller) ) {

		std::vector< OmSegment* > segmentsToDraw;

		if( !mSegmentCache->segmentListDirectCacheHasCoord( chunkCoord ) ){
			const OmSegIDsSet & chunkValues =  p_chunk->GetDirectDataValues();
			OmSegment * seg = segIter.getNextSegment();
			OmSegID val;
			while( NULL != seg ){
				val = seg->getValue();
				if( chunkValues.contains( val ) ){
					// TODO: wire size threshold into gui (purcaro)
					//  make sure to clear the segmentListDirectCacheHasCoord if needed!
					if( seg->getSize() > 100 ){
						segmentsToDraw.push_back(seg);
					}
				}
				
				seg = segIter.getNextSegment();
			}
			mSegmentCache->setSegmentListDirectCache( chunkCoord, segmentsToDraw );
			//printf("segmentsToDraw=%i\n", segmentsToDraw.size());
		}

		return DrawChunk(p_chunk, chunkCoord, mSegmentCache->getSegmentListDirectCache( chunkCoord ), rCuller);
	}

	// ELSE BREAK DOWN INTO CHILDREN

	const set<OmMipChunkCoord> & coords = p_chunk->GetChildrenCoordinates();
	std::set<OmMipChunkCoord>::const_iterator iter;
	for( iter = coords.begin(); iter != coords.end(); ++iter ){
		DrawChunkRecursive(*iter, segIter, testVis, rCuller);
	}
}

/*
 *	MipChunk determined to be visible so draw contents depending on mode.
 */
void OmMeshDrawer::DrawChunk(QExplicitlySharedDataPointer < OmMipChunk > p_chunk,
			       const OmMipChunkCoord & chunkCoord,	
			       const OmSegPtrs & segmentsToDraw,
			       OmVolumeCuller & rCuller)
{
	if( segmentsToDraw.empty() ){
		return;
	}

	//draw extent
	if (rCuller.CheckDrawOption(DRAWOP_DRAW_CHUNK_EXTENT)) {
		p_chunk->DrawClippedExtent();
	}

	//if not set to render segments
	if (!rCuller.CheckDrawOption(DRAWOP_LEVEL_SEGMENT)) {
		return;
	}

	mSeg->mMipMeshManager.DrawMeshes(rCuller.GetDrawOptions(), 
					chunkCoord, 
					segmentsToDraw, mViewGroupState);
}

#include "mesh/omMeshDrawer.h"
#include "common/omGl.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentIterator.h"
#include "volume/omMipChunk.h"
#include "volume/omVolumeCuller.h"
#include "system/viewGroup/omViewGroupState.h"

OmMeshDrawer::OmMeshDrawer(OmSegmentation * seg, OmViewGroupState * vgs )
	: mSeg(seg)
	, mSegmentCache(seg->mSegmentCache)
	, mViewGroupState(vgs)
{
	assert(mSeg);
	assert(mSegmentCache);
	assert(mViewGroupState);
	
}

/////////////////////////////////
///////          Draw

/*
 *	Draw the entire Segmentation.  Starts the recursive processes of drawing MipChunks
 *	from the root MipChunk of the Segmentation.  Filters for relevant data values to be 
 *	drawn depending on culler draw options and passes relevant set to root chunk.
 */
void OmMeshDrawer::Draw(OmVolumeCuller & rCuller)
{
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

	DrawMeshes(rCuller.GetDrawOptions(), 
		   chunkCoord, 
		   segmentsToDraw);
}

////////////////////////////////
///////          Draw

void OmMeshDrawer::DrawMeshes(const OmBitfield & drawOps,
			      const OmMipChunkCoord & mipCoord, 
			      const OmSegPtrs  & segmentsToDraw )
{
	std::vector<OmSegment*>::const_iterator iter;
	for( iter = segmentsToDraw.begin(); iter != segmentsToDraw.end(); ++iter ){

		//get pointer to mesh
		QExplicitlySharedDataPointer < OmMipMesh > p_mesh = QExplicitlySharedDataPointer < OmMipMesh > ();
		mSeg->mMipMeshManager.GetMesh(p_mesh, OmMipMeshCoord(mipCoord, (*iter)->getValue() ));

		if (NULL == p_mesh) {
			continue;
		}

		//apply segment color
		//(*iter)->ApplyColor(drawOps, vgs);
		mViewGroupState->ColorMesh(drawOps, *iter);

		//draw mesh
		glPushName((*iter)->getValue());
		glPushName(OMGL_NAME_MESH);

		p_mesh->Draw(true);

		glPopName();
		glPopName();

	}
}

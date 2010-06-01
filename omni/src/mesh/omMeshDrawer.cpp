#include "mesh/omMeshDrawer.h"
#include "common/omGl.h"
#include "project/omProject.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentIterator.h"
#include "volume/omMipChunk.h"
#include "system/viewGroup/omViewGroupState.h"
#include "volume/omVolumeCuller.h"

static unsigned int mFreshness = 0;
static boost::unordered_map< int, 
	boost::unordered_map< int,
	 boost::unordered_map< int,
	  boost::unordered_map< int, OmSegPtrsValid > > > > mSegmentListCache;

// NOTE: I am assuming this class is only being used in a single-threaded fashion..

OmMeshDrawer::OmMeshDrawer( const OmId segmentationID, OmViewGroupState * vgs )
	: mSeg(&OmProject::GetSegmentation(segmentationID))
	, mSegmentCache(mSeg->mSegmentCache)
	, mViewGroupState(vgs)
	, mIterOverSelectedIDs( false )
	, mIterOverEnabledIDs( false )
{
	assert(mSeg);
	assert(mSegmentCache);
	assert(mViewGroupState);
}

OmMeshDrawer::~OmMeshDrawer()
{
	delete mVolumeCuller;
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
	checkCache();

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
	mVolumeCuller = rCuller.GetTransformedCuller(mSeg->mNormToSpaceMat, 
						     mSeg->mNormToSpaceInvMat);


	//check to filter for relevant data values
	if (rCuller.CheckDrawOption(DRAWOP_SEGMENT_FILTER_SELECTED)) {
		mIterOverSelectedIDs = true;
		
	} else if (rCuller.CheckDrawOption(DRAWOP_SEGMENT_FILTER_UNSELECTED)) {
		mIterOverEnabledIDs = true;
	}

	OmSegmentIterator iter(mSegmentCache, mIterOverSelectedIDs, mIterOverEnabledIDs );
	if( iter.empty() ){
        	glPopMatrix();
		return;
	}

	glPushName(mSeg->GetId());

	//draw relevant data values starting from root chunk
	DrawChunkRecursive(mSeg->RootMipChunkCoordinate(), true );

	glPopName();

        //pop matrix
        glPopMatrix();
}

/*
 *	Recursively draw MipChunks within the Segmentation using the MipCoordinate hierarchy.
 *	Uses the OmVolumeCuller to determine the visibility of a MipChunk.  If visible, the
 *	MipChunk is either drawn or the recursive draw process is called on its children.
 */
void OmMeshDrawer::DrawChunkRecursive(const OmMipChunkCoord & chunkCoord, bool testVis)
{
	// get pointer to chunk
	QExplicitlySharedDataPointer < OmMipChunk > p_chunk = QExplicitlySharedDataPointer < OmMipChunk > ();
	mSeg->GetChunk(p_chunk, chunkCoord);

	// TEST FOR CHUNK VISIBILITY IF NECESSARY
	if (testVis) {
		//check if frustum contains chunk
		switch (mVolumeCuller->TestChunk(*p_chunk)) {
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
	if ( DrawCheck(p_chunk) ) {
		DrawChunk( p_chunk, chunkCoord );
		return;
	}

	// ELSE BREAK DOWN INTO CHILDREN

	const set<OmMipChunkCoord> & coords = p_chunk->GetChildrenCoordinates();
	std::set<OmMipChunkCoord>::const_iterator iter;
	for( iter = coords.begin(); iter != coords.end(); ++iter ){
		DrawChunkRecursive(*iter, testVis);
	}
}

void OmMeshDrawer::populateSegIDsCache(QExplicitlySharedDataPointer < OmMipChunk > p_chunk,
				       const OmMipChunkCoord & chunkCoord)
{
	// TODO: refactor segmentListDirectCacheHasCoord out of SegmentCache....

	const OmSegIDsSet & chunkValues =  p_chunk->GetDirectDataValues();
	OmSegmentIterator segIter(mSegmentCache, mIterOverSelectedIDs, mIterOverEnabledIDs );
	OmSegment * seg = segIter.getNextSegment();
	OmSegID val;

	OmSegPtrs segmentsToDraw;

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

	setSegmentListDirectCache( chunkCoord, segmentsToDraw );
	//printf("segmentsToDraw=%i\n", segmentsToDraw.size());
}

/*
 *	MipChunk determined to be visible so draw contents depending on mode.
 */
void OmMeshDrawer::DrawChunk(QExplicitlySharedDataPointer < OmMipChunk > p_chunk,
			     const OmMipChunkCoord & chunkCoord)
{
	// figure out which segments to draw
	if( !segmentListDirectCacheHasCoord( chunkCoord ) ){
		populateSegIDsCache( p_chunk, chunkCoord );
	}

	const OmSegPtrs & segmentsToDraw = getSegmentListDirectCache( chunkCoord );

	if( segmentsToDraw.empty() ){
		return;
	}

	//draw extent
	if (mVolumeCuller->CheckDrawOption(DRAWOP_DRAW_CHUNK_EXTENT)) {
		p_chunk->DrawClippedExtent();
	}

	//if not set to render segments
	if (!mVolumeCuller->CheckDrawOption(DRAWOP_LEVEL_SEGMENT)) {
		return;
	}

	DrawMeshes(mVolumeCuller->GetDrawOptions(), 
		   chunkCoord, 
		   segmentsToDraw);
}

////////////////////////////////
///////          Draw

void OmMeshDrawer::DrawMeshes(const OmBitfield & drawOps,
			      const OmMipChunkCoord & mipCoord, 
			      const OmSegPtrs & segmentsToDraw )
{
	OmSegPtrs::const_iterator iter;
	for( iter = segmentsToDraw.begin(); iter != segmentsToDraw.end(); ++iter ){

		//get pointer to mesh
		QExplicitlySharedDataPointer < OmMipMesh > p_mesh = QExplicitlySharedDataPointer < OmMipMesh > ();
		mSeg->mMipMeshManager.GetMesh(p_mesh, OmMipMeshCoord(mipCoord, (*iter)->getValue() ));

		if (NULL == p_mesh) {
			continue;
		}

		//apply segment color
		mViewGroupState->ColorMesh(drawOps, *iter);

		//draw mesh
		glPushName((*iter)->getValue());
		glPushName(OMGL_NAME_MESH);

		p_mesh->Draw(true);

		glPopName();
		glPopName();
	}
}

/*
 *	Given that the chunk is visible, determine if it should be drawn
 *	or if we should continue refining so as to draw children.
 */
bool OmMeshDrawer::DrawCheck(QExplicitlySharedDataPointer < OmMipChunk > p_chunk)
{
	//draw if leaf
	if(p_chunk->IsLeaf()) {
		return true;
	}
	
	const NormBbox & normExtent = p_chunk->GetNormExtent();
	const NormBbox & clippedNormExtent = p_chunk->GetClippedNormExtent();

	NormCoord camera = mVolumeCuller->GetPosition();
	NormCoord center = clippedNormExtent.getCenter();

	float camera_to_center = center.distance(camera);
	float distance = (normExtent.getMax() - normExtent.getCenter()).length();

	//if distance too large, just draw it - else keep breaking it down
	debug("vol", "cam,dist:%f,%f\n", camera_to_center, distance);
	return (camera_to_center > distance);
}

// TODO: hashes could just be replaced by 3D array, where each dimension is the number of chunks in that dimension (purcaro)
void OmMeshDrawer::setSegmentListDirectCache( const OmMipChunkCoord & c,
					      const OmSegPtrs & segmentsToDraw )
{
	mSegmentListCache[c.Level][c.Coordinate.x][c.Coordinate.y][c.Coordinate.z] = OmSegPtrsValid( segmentsToDraw );
}

bool OmMeshDrawer::segmentListDirectCacheHasCoord( const OmMipChunkCoord & c )
{
	OmSegPtrsValid & spList = mSegmentListCache[c.Level][c.Coordinate.x][c.Coordinate.y][c.Coordinate.z];
	return spList.isValid;
}

const OmSegPtrs & OmMeshDrawer::getSegmentListDirectCache( const OmMipChunkCoord & c )
{
	const OmSegPtrsValid & spList = mSegmentListCache[c.Level][c.Coordinate.x][c.Coordinate.y][c.Coordinate.z];
	return spList.list;
}

void OmMeshDrawer::checkCache()
{
	const unsigned int currentFreshness = OmCacheManager::Freshen(false);

	if( currentFreshness != mFreshness ){
		mSegmentListCache.clear();
		mFreshness = currentFreshness;
	}
}

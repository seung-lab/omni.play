#include "common/omGl.h"
#include "mesh/omMeshDrawer.h"
#include "project/omProject.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentIterator.h"
#include "system/omLocalPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omPreferences.h"
#include "system/viewGroup/omViewGroupState.h"
#include "volume/omMipChunk.h"
#include "volume/omVolumeCuller.h"

static unsigned int mFreshness = 0;
static boost::unordered_map< OmId, 
        boost::unordered_map< int, 
	 boost::unordered_map< int,
	  boost::unordered_map< int,
 	   boost::unordered_map< int, OmSegPtrListValid > > > > > mSegmentListCache;

// NOTE: I am assuming this class is only being used in a single-threaded fashion..

OmMeshDrawer::OmMeshDrawer( const OmId segmentationID, OmViewGroupState * vgs )
	: mSegmentationID( segmentationID )
	, mSeg( NULL )
	, mSegmentCache( NULL )
	, mViewGroupState(vgs)
	, mIterOverSelectedIDs( false )
	, mIterOverEnabledIDs( false )
{
}

OmMeshDrawer::~OmMeshDrawer()
{
	delete mVolumeCuller;
}

void OmMeshDrawer::Init()
{
	mSeg = &OmProject::GetSegmentation(mSegmentationID);
	assert(mSeg);

	mSegmentCache = mSeg->GetSegmentCache();
	assert(mSegmentCache);

	assert(mViewGroupState);

	checkCache();
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
	glMultMatrixf(mSeg->GetNormToSpaceMatrix().ml);

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
	mVolumeCuller = rCuller.GetTransformedCuller(mSeg->GetNormToSpaceMatrix(), 
						     mSeg->GetNormToSpaceInvMatrix());

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

	glPushName( mSegmentationID );

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
	OmMipChunkPtr p_chunk = OmMipChunkPtr ();
	mSeg->GetChunk(p_chunk, chunkCoord);

	// test for chunk visibility (if necessary)
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
	
	if ( ShouldChunkBeDrawn(p_chunk) ) {
		
		// if allowed to render segments
		// TODO: do we really need this option? (purcaro)
		if( mVolumeCuller->CheckDrawOption(DRAWOP_LEVEL_SEGMENT) ){
			DrawChunk( p_chunk, chunkCoord );
		}

	} else {
		const set<OmMipChunkCoord> & coords = p_chunk->GetChildrenCoordinates();
		std::set<OmMipChunkCoord>::const_iterator iter;
		for( iter = coords.begin(); iter != coords.end(); ++iter ){
			DrawChunkRecursive(*iter, testVis);
		}
	}
}

void OmMeshDrawer::makeSegmentListForCache(OmMipChunkPtr p_chunk, const OmMipChunkCoord & chunkCoord)
{
	const OmSegIDsSet & chunkValues =  p_chunk->GetDirectDataValues();
	OmSegmentIterator segIter(mSegmentCache, mIterOverSelectedIDs, mIterOverEnabledIDs );
	OmSegment * seg = segIter.getNextSegment();
	OmSegID val;

	OmSegPtrList segmentsToDraw;

	while( NULL != seg ){
		val = seg->getValue();
		if( chunkValues.contains( val ) ){
			segmentsToDraw.push_back(seg);
		}
				
		seg = segIter.getNextSegment();
	}

	addToCache( chunkCoord, segmentsToDraw );
}

/*
 *	MipChunk determined to be visible so draw contents depending on mode.
 */
void OmMeshDrawer::DrawChunk(OmMipChunkPtr p_chunk, const OmMipChunkCoord & chunkCoord)
{
	if( !cacheHasCoord( chunkCoord ) ){
		makeSegmentListForCache( p_chunk, chunkCoord );
	}

	const OmSegPtrList & segmentsToDraw = getFromCache( chunkCoord );

	if( segmentsToDraw.empty() ){
		return;
	}

	if (mVolumeCuller->CheckDrawOption(DRAWOP_DRAW_CHUNK_EXTENT)) {
		DrawClippedExtent( p_chunk );
	}

	OmSegPtrList::const_iterator iter;
	for( iter = segmentsToDraw.begin(); iter != segmentsToDraw.end(); ++iter ){
		
		// TODO: wire size threshold into gui (purcaro)
		if( (*iter)->getSize() < mViewGroupState->getDustThreshold() ){
			continue;
		}

		//get pointer to mesh
		QExplicitlySharedDataPointer < OmMipMesh > p_mesh = QExplicitlySharedDataPointer < OmMipMesh > ();
		mSeg->mMipMeshManager.GetMesh(p_mesh, OmMipMeshCoord(chunkCoord, (*iter)->getValue() ));

		if (NULL == p_mesh) {
			continue;
		}

		//apply segment color
		ColorMesh(mVolumeCuller->GetDrawOptions(), *iter);

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
bool OmMeshDrawer::ShouldChunkBeDrawn(OmMipChunkPtr p_chunk)
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
void OmMeshDrawer::addToCache( const OmMipChunkCoord & c,
			       const OmSegPtrList & segmentsToDraw )
{
	mSegmentListCache[ mSegmentationID ][c.Level][c.Coordinate.x][c.Coordinate.y][c.Coordinate.z] = OmSegPtrListValid( segmentsToDraw );
}

bool OmMeshDrawer::cacheHasCoord( const OmMipChunkCoord & c )
{
	OmSegPtrListValid & spList = mSegmentListCache[ mSegmentationID ][c.Level][c.Coordinate.x][c.Coordinate.y][c.Coordinate.z];
	return spList.isValid;
}

const OmSegPtrList & OmMeshDrawer::getFromCache( const OmMipChunkCoord & c )
{
	const OmSegPtrListValid & spList = mSegmentListCache[ mSegmentationID ][c.Level][c.Coordinate.x][c.Coordinate.y][c.Coordinate.z];
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

void OmMeshDrawer::DrawClippedExtent(OmMipChunkPtr p_chunk)
{
	return;

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();

	//disable lighting for lines
	glDisable(GL_LIGHTING);

	const NormBbox & clippedNormExtent = p_chunk->GetClippedNormExtent();

	//translate and scale to chunk norm extent
	Vector3f translate = clippedNormExtent.getMin();
	Vector3f scale = clippedNormExtent.getMax() - clippedNormExtent.getMin();

	//transform model view
	glTranslatefv(translate.array);
	glScalefv(scale.array);

	glTranslatef(0.5, 0.5, 0.5);
	glColor3f(0.5, 0.5, 0.5);
	//omglWireCube(1);
	glTranslatef(-0.5, -0.5, -0.5);

	//glScalefv( (Vector3f::ONE/scale).array);
	//glTranslatefv( (-translate).array);

	glPopMatrix();
	glPopAttrib();
}

void OmMeshDrawer::ColorMesh(const OmBitfield & drawOps, OmSegment * segment)
{
        OmSegmentColorCacheType sccType;

        if( mViewGroupState->shouldMeshBeShownBroken() ) {
        	sccType = SegmentationBreak;
        } else {
        	sccType = Segmentation;
        }

	ApplyColor( segment, drawOps, sccType);
}

void OmMeshDrawer::ApplyColor(OmSegment * seg, const OmBitfield & drawOps, 
			      const OmSegmentColorCacheType sccType)
{
	if( seg->getParentSegID() && sccType != SegmentationBreak){
		ApplyColor(mSegmentCache->findRoot(seg), drawOps, sccType);
		return;
	}

	Vector3<float> hyperColor = seg->GetColorFloat();

	hyperColor.x *= 2.;
	hyperColor.y *= 2.;
	hyperColor.z *= 2.;

	//check coloring options
	if (drawOps & DRAWOP_SEGMENT_COLOR_HIGHLIGHT) {
		glColor3fv(OmPreferences::GetVector3f(OM_PREF_VIEW3D_HIGHLIGHT_COLOR_V3F).array);

	} else if (drawOps & DRAWOP_SEGMENT_COLOR_TRANSPARENT) {
		glColor3fva(hyperColor.array, 
			    OmPreferences::GetFloat(OM_PREF_VIEW3D_TRANSPARENT_ALPHA_FLT));

	} else if (OmLocalPreferences::getDoDiscoBall()) {
		static float s = 10.0;
		static int dir = 1;
		
		glEnable(GL_BLEND);
		glColor3fva(hyperColor.array, (s)/200+.4);
		s += .1*dir;
		if (s > 60) dir = -1;
		if (s < 10) dir = 1;
		glMaterialf(GL_FRONT, GL_SHININESS, 100-s);

	} else {
		glColor3fv(hyperColor.array);
	}
}

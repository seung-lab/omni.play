#include "common/omDebug.h"
#include "common/omGl.h"
#include "mesh/omMeshDrawer.h"
#include "mesh/omMeshSegmentList.h"
#include "mesh/omMipMesh.h"
#include "project/omProject.h"
#include "segment/omSegmentCache.h"
#include "system/omEvents.h"
#include "system/omGarbage.h"
#include "system/omLocalPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omPreferences.h"
#include "viewGroup/omViewGroupState.h"
#include "volume/omMipChunk.h"
#include "volume/omSegmentation.h"
#include "mesh/omVolumeCuller.h"

OmMeshDrawer::OmMeshDrawer(OmSegmentation* seg, OmViewGroupState* vgs)
	: mSeg(seg)
	, mViewGroupState(vgs)
	, mSegmentCache(mSeg->GetSegmentCache())
	, redrawNeeded_(false)
{
}

/////////////////////////////////
///////          Draw

/*
 *	Draw the entire Segmentation.  Starts the recursive processes of drawing MipChunks
 *	from the root MipChunk of the Segmentation.  Filters for relevant data values to be
 *	drawn depending on culler draw options and passes relevant set to root chunk.
 */
void OmMeshDrawer::Draw(OmVolumeCuller& rCuller)
{
	OmGarbage::CleanGenlists();

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
		FOR_EACH(iter, mSegmentCache->GetSelectedSegmentIds()){
			mRootSegsToDraw.push_back(mSegmentCache->GetSegment(*iter));
		}
	} else if (rCuller.CheckDrawOption(DRAWOP_SEGMENT_FILTER_UNSELECTED)) {
		FOR_EACH(iter, mSegmentCache->GetEnabledSegmentIds()){
			mRootSegsToDraw.push_back(mSegmentCache->GetSegment(*iter));
		}
	}

	if( mRootSegsToDraw.empty() ){
		glPopMatrix();
		return;
	}

	glPushName(mSeg->GetID());

	//draw relevant data values starting from root chunk
	drawChunkRecursive(mSeg->RootMipChunkCoordinate(), true);

	glPopName();

	//pop matrix
	glPopMatrix();

	if(redrawNeeded_){
		OmEvents::Redraw3d();
	}
}

/*
 *	Recursively draw MipChunks within the Segmentation using the MipCoordinate hierarchy.
 *	Uses the OmVolumeCuller to determine the visibility of a MipChunk.  If visible, the
 *	MipChunk is either drawn or the recursive draw process is called on its children.
 */
void OmMeshDrawer::drawChunkRecursive(const OmMipChunkCoord& chunkCoord,
									  bool testVis)
{
	OmMipChunkPtr chunk = OmMipChunkPtr();
	mSeg->GetChunk(chunk, chunkCoord);

	// test for chunk visibility (if necessary)
	if(testVis) {
		//check if frustum contains chunk
		switch (mVolumeCuller->TestChunk(chunk->GetNormExtent())) {
		case VISIBILITY_NONE:
			return;

		case VISIBILITY_PARTIAL:  //continue drawing tree and continue testing children
			break;

		case VISIBILITY_FULL:	  //continue drawing tree, but assume children are visible
			testVis = false;
			break;
		}
	}

	if( shouldChunkBeDrawn(chunk) ){

		// if allowed to render segments
		// TODO: do we really need this option? (purcaro)
		if( mVolumeCuller->CheckDrawOption(DRAWOP_LEVEL_SEGMENT) ){
			drawChunk(chunk);
		}

	} else {
		FOR_EACH(iter, chunk->GetChildrenCoordinates()){
			drawChunkRecursive(*iter, testVis);
		}
	}
}

/*
 *	MipChunk determined to be visible so draw contents depending on mode.
 */
void OmMeshDrawer::drawChunk(OmMipChunkPtr chunk)
{
	bool segsWereFound = false;

	FOR_EACH(iter, mRootSegsToDraw){
		OmSegment* rootSeg = (*iter);

		boost::optional<OmSegPtrList> segmentsToDraw =
			OmMeshSegmentList::getFromCacheIfReady(chunk, rootSeg);

		if(!segmentsToDraw){
			continue;
		}

		if(segmentsToDraw->empty()){
			continue;
		}

		segsWereFound = true;
		doDrawChunk(chunk->GetCoordinate(), *segmentsToDraw);
	}

	if(segsWereFound){
		if (mVolumeCuller->CheckDrawOption(DRAWOP_DRAW_CHUNK_EXTENT)) {
			// draw bounding box around chunk
			drawClippedExtent(chunk);
		}
	}
}

void OmMeshDrawer::doDrawChunk(const OmMipChunkCoord& chunkCoord,
							   const OmSegPtrList& segmentsToDraw)
{
	FOR_EACH(iter, segmentsToDraw ){
		OmSegment* seg = *iter;

		if(seg->getSize() < mViewGroupState->getDustThreshold()){
			continue;
		}

		OmMipMeshPtr p_mesh;
		mSeg->GetMesh(p_mesh, chunkCoord, seg->value());

		if(!p_mesh){
			redrawNeeded_ = true;
			continue;
		}

		if(!p_mesh->hasData()){
			continue;
		}

		//apply segment color
		colorMesh(mVolumeCuller->GetDrawOptions(), *iter);

		//draw mesh
		glPushName(seg->value());
		glPushName(OMGL_NAME_MESH);

		p_mesh->Draw();

		glPopName();
		glPopName();
	}
}

/*
 *	Given that the chunk is visible, determine if it should be drawn
 *	or if we should continue refining so as to draw children.
 */
bool OmMeshDrawer::shouldChunkBeDrawn(OmMipChunkPtr p_chunk)
{
	// draw if MIP 0
	if(0 == p_chunk->GetCoordinate().Level){
		return true;
	}

	const NormBbox& normExtent = p_chunk->GetNormExtent();
	const NormBbox& clippedNormExtent = p_chunk->GetClippedNormExtent();

	const NormCoord camera = mVolumeCuller->GetPosition();
	const NormCoord center = clippedNormExtent.getCenter();

	const float camera_to_center = center.distance(camera);
	const float distance =
		(normExtent.getMax() - normExtent.getCenter()).length();

	//if distance too large, just draw it - else keep breaking it down
	return (camera_to_center > distance);
}

void OmMeshDrawer::drawClippedExtent(OmMipChunkPtr p_chunk)
{
	return; // FIXME!

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();

	//disable lighting for lines
	glDisable(GL_LIGHTING);

	const NormBbox & clippedNormExtent = p_chunk->GetClippedNormExtent();

	//translate and scale to chunk norm extent
	const Vector3f translate = clippedNormExtent.getMin();
	const Vector3f scale =
		clippedNormExtent.getMax() - clippedNormExtent.getMin();

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

void OmMeshDrawer::colorMesh(const OmBitfield & drawOps, OmSegment * segment)
{
	OmSegmentColorCacheType sccType;

	if( mViewGroupState->shouldVolumeBeShownBroken() ) {
		sccType = SCC_SEGMENTATION_BREAK;
	} else {
		sccType = SCC_SEGMENTATION;
	}

	applyColor( segment, drawOps, sccType);
}

void OmMeshDrawer::applyColor(OmSegment * seg, const OmBitfield & drawOps,
							  const OmSegmentColorCacheType sccType)
{
	if( seg->getParentSegID() && sccType != SCC_SEGMENTATION_BREAK){
		applyColor(mSegmentCache->findRoot(seg), drawOps, sccType);
		return;
	}

	const Vector3f hyperColor = seg->GetColorFloat() * 2.;

	//check coloring options
	if (drawOps & DRAWOP_SEGMENT_COLOR_HIGHLIGHT) {
		glColor3fv(OmPreferences::GetVector3f(om::PREF_VIEW3D_HIGHLIGHT_COLOR_V3F).array);

	} else if (drawOps & DRAWOP_SEGMENT_COLOR_TRANSPARENT) {
		glColor3fva(hyperColor.array,
					OmPreferences::GetFloat(om::PREF_VIEW3D_TRANSPARENT_ALPHA_FLT));

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

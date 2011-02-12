#ifndef OM_MESH_DRAWER_IMPL_HPP
#define OM_MESH_DRAWER_IMPL_HPP

#include "common/omCommon.h"
#include "common/omDebug.h"
#include "common/omGl.h"
#include "mesh/drawer/omMeshPlan.h"
#include "mesh/omMipMesh.h"
#include "mesh/omMipMeshManagers.hpp"
#include "mesh/omVolumeCuller.h"
#include "segment/omSegmentPointers.h"
#include "segment/omSegmentUtils.hpp"
#include "system/omEvents.h"
#include "system/omGarbage.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omPreferences.h"
#include "utility/omTimer.hpp"
#include "view3d/om3dPreferences.hpp"
#include "viewGroup/omViewGroupState.h"
#include "chunks/omChunk.h"

class OmMeshDrawerImpl {
private:
	OmSegmentation *const segmentation_;
	OmViewGroupState *const vgs_;
	OmSegmentCache *const mSegmentCache;
	const OmBitfield drawOptions_;
	OmMeshPlan *const sortedSegments_;

	OmTimer elapsed_;

	bool redrawNeeded_;
	uint32_t numSegsDrawn_;
	uint64_t numVoxelsDrawn_;

public:
	OmMeshDrawerImpl(OmSegmentation* seg, OmViewGroupState* vgs,
					 const OmBitfield drawOptions,
					 OmMeshPlan* sortedSegments)
		: segmentation_(seg)
		, vgs_(vgs)
		, mSegmentCache(segmentation_->SegmentCache())
		, drawOptions_(drawOptions)
		, sortedSegments_(sortedSegments)
		, redrawNeeded_(false)
		, numSegsDrawn_(0)
		, numVoxelsDrawn_(0)
	{}

	/**
	 *	Draw the mesh plan. Filters for relevant data values to be
	 *	drawn depending on culler draw options
	 */
	void Draw(const int allowedDrawTime)
	{
		OmGarbage::CleanGenlists();

		//transform to normal frame
		glPushMatrix();
		glMultMatrixf(segmentation_->Coords().GetNormToSpaceMatrix().ml);

		//draw volume axis
		if(checkDrawOption(DRAWOP_DRAW_VOLUME_AXIS)) {
			glDrawPositiveAxis();
		}

		//return if no chunk level drawing
		if (!checkDrawOption(DRAWOP_LEVEL_CHUNKS)) {
			glPopMatrix();
			return;
		}

		if(sortedSegments_->empty() ){
			glPopMatrix();
			return;
		}

		glPushName(segmentation_->GetID());

		drawPlan(allowedDrawTime);

		glPopName();

		glPopMatrix();

		if(redrawNeeded_){
			OmEvents::Redraw3d();
		}
	}

	uint32_t NumSegsDrawn() const {
		return numSegsDrawn_;
	}

	uint32_t TotalNumSegmentsPresent() const {
		return sortedSegments_->size();
	}

	uint64_t NumVoxelsDrawn() const {
		return numVoxelsDrawn_;
	}

	uint64_t TotalVoxelsPresent() const {
		return sortedSegments_->TotalVoxels();
	}

	bool RedrawNeeded() const {
		return redrawNeeded_;
	}

private:

	inline bool checkDrawOption(const OmBitfield option){
		return drawOptions_ & option;
	}

	void drawPlan(const int allowedDrawTime)
	{
		FOR_EACH(iter, *sortedSegments_){

			if(elapsed_.ms_elapsed() > allowedDrawTime) {
				redrawNeeded_ = true;
				return;
			}

			drawSegment(iter->first, iter->second);
		}
	}

	void drawSegment(OmSegment* seg, const OmChunkCoord& coord)
	{
		OmMipMeshPtr mesh;
		segmentation_->MeshManagers()->GetMesh(mesh, coord, seg->value(), 1);

		if(!mesh){
			redrawNeeded_ = true;
			return;
		}

		if(!mesh->HasData()){
			return;
		}

		++numSegsDrawn_;
		numVoxelsDrawn_ += seg->size();

		//apply segment color
		colorMesh(seg);

		//draw mesh
		glPushName(seg->value());
		glPushName(OMGL_NAME_MESH);

		mesh->Draw();

		glPopName();
		glPopName();
	}

	// draw chunk bounding box--broken? (purcaro)
	void drawClippedExtent(OmChunkPtr chunk)
	{
		return; // FIXME!

		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glPushMatrix();

		//disable lighting for lines
		glDisable(GL_LIGHTING);

		const NormBbox& clippedNormExtent = chunk->Mipping().GetClippedNormExtent();

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

	inline void colorMesh(OmSegment* segment)
	{
		OmSegmentColorCacheType sccType;

		if( vgs_->shouldVolumeBeShownBroken() ) {
			sccType = SCC_SEGMENTATION_BREAK;
		} else {
			sccType = SCC_SEGMENTATION;
		}

		applyColor( segment, sccType);
	}

	void applyColor(OmSegment * seg, const OmSegmentColorCacheType sccType)
	{
		if(seg->getParent() && sccType != SCC_SEGMENTATION_BREAK){
			applyColor(mSegmentCache->findRoot(seg), sccType);
			return;
		}

		Vector3f hyperColor;
		if(SCC_SEGMENTATION_BREAK != sccType) {
			hyperColor = seg->GetColorFloat() * 2.;
		} else {
			if(OmSegmentUtils::UseParentColorBasedOnThreshold(seg, vgs_)){
				// WARNING: recusive operation is O(depth of MST)
				applyColor(seg->getParent(), sccType);
				return;
			}
			hyperColor = seg->GetColorFloat();
		}

		//check coloring options
		if(checkDrawOption(DRAWOP_SEGMENT_COLOR_HIGHLIGHT)){
			glColor3fv(OmPreferences::GetVector3f(om::PREF_VIEW3D_HIGHLIGHT_COLOR_V3F).array);

		} else if (checkDrawOption(DRAWOP_SEGMENT_COLOR_TRANSPARENT)){
			glColor3fva(hyperColor.array,
						OmPreferences::GetFloat(om::PREF_VIEW3D_TRANSPARENT_ALPHA_FLT));

		} else if (Om3dPreferences::getDoDiscoBall()){
			showAsDiscoBall(hyperColor);
		} else {
			glColor3fv(hyperColor.array);
		}
	}

	inline void showAsDiscoBall(const Vector3f& hyperColor)
	{
		static float s = 10.0;
		static int dir = 1;

		glEnable(GL_BLEND);
		glColor3fva(hyperColor.array, (s)/200+.4);

		s += .1*dir;

		if (s > 60){
			dir = -1;
		}
		if (s < 10) {
			dir = 1;
		}

		glMaterialf(GL_FRONT, GL_SHININESS, 100-s);
	}
};
#endif

#include "viewGroup/omBrushSize.hpp"
#include "project/omProject.h"
#include "segment/omSegmentSelected.hpp"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentSelector.h"
#include "tiles/cache/omTileCache.h"
#include "system/omStateManager.h"
#include "viewGroup/omViewGroupState.h"
#include "utility/dataWrappers.h"
#include "view2d/omLineDraw.hpp"
#include "view2d/omView2dState.hpp"
#include "volume/omSegmentation.h"
#include "actions/omActions.hpp"

OmLineDraw::OmLineDraw(boost::shared_ptr<OmView2dState> v2ds,
					   const ViewType vt)
	: state_(v2ds)
	, mViewType(vt)
{
	mDoRefresh = false;
	mCurrentSegmentId = 0;
	mEditedSegmentation = 0;
}

void OmLineDraw::DrawLine(const DataCoord& /*startPoint*/,
						  const DataCoord& /*endPoint*/)
{
}

void OmLineDraw::bresenhamLineDraw(const DataCoord & first,
								   const DataCoord & second,
								   bool doselection)
{
	//store current selection
	SegmentDataWrapper sdw = OmSegmentSelected::Get();

	//return if not valid
	if (!sdw.IsSegmentValid())
		return;

	const OmID segmentation_id = sdw.GetSegmentationID();

	//switch on tool mode
	OmSegID data_value = 0;
	switch (OmStateManager::GetToolMode()) {
	case ADD_VOXEL_MODE:
		//get value associated to segment id
		data_value = sdw.getID();
		break;

	case SUBTRACT_VOXEL_MODE:
		data_value = 0;
		break;

	default:
		//assert(false);
		break;
	}

	const float mDepth = state_->getViewGroupState()->GetViewSliceDepth(mViewType);
	const DataCoord data_coord = state_->getVol()->SpaceToDataCoord(SpaceCoord(0, 0, mDepth));

	DataCoord globalDC;
	int y1, y0, x1, x0;

	switch (mViewType) {
	case XY_VIEW:
		y1 = second.y;
		y0 = first.y;
		x1 = second.x;
		x0 = first.x;
		break;
	case XZ_VIEW:
		y1 = second.z;
		y0 = first.z;
		x1 = second.x;
		x0 = first.x;
		break;
	case YZ_VIEW:
		y1 = second.y;
		y0 = first.y;
		x1 = second.z;
		x0 = first.z;
		break;
	}

	int dy = y1 - y0;
	int dx = x1 - x0;
	int stepx, stepy;

	if (dy < 0) {
		dy = -dy;
		stepy = -1;
	} else {
		stepy = 1;
	}
	if (dx < 0) {
		dx = -dx;
		stepx = -1;
	} else {
		stepx = 1;
	}
	dy <<= 1;		// dy is now 2*dy
	dx <<= 1;		// dx is now 2*dx

	//debug(brush, "coords: %i,%i,%i\n", x0, y0, mViewDepth);
	//debug(brush, "mDepth = %f\n", mDepth);

	OmSegmentSelector sel(SegmentationDataWrapper(segmentation_id), this, "view2d_selector" );
	OmSegmentation & current_seg = OmProject::GetSegmentation(segmentation_id);

	if(doselection) {
		PickToolAddToSelection(sel, current_seg, first);
	}

	boost::shared_ptr<OmBrushSize>& brushSize = state_->getBrushSize();

	if (dx > dy) {
		int fraction = dy - (dx >> 1);	// same as 2*dy - dx
		while (x0 != x1) {
			if (fraction >= 0) {
				y0 += stepy;
				fraction -= dx;	// same as fraction -= 2*dx
			}
			x0 += stepx;
			fraction += dy;	// same as fraction -= 2*dy

			// do the conversion ourselves; painting "skips" otherwise
			DataCoord globalDC;
			switch (mViewType) {
			case XY_VIEW:
				globalDC = DataCoord(x0, y0, second.z);
				break;
			case XZ_VIEW:
				globalDC = DataCoord(x0, second.y, y0);
				break;
			case YZ_VIEW:
				globalDC = DataCoord(second.x, y0, x0);
				break;
			}

			if(brushSize->Diameter() > 4 &&
			   (x1 == x0 || abs(x1 - x0) % (brushSize->Diameter() / 4) == 0)) {
				if (!doselection){
					BrushToolApplyPaint(segmentation_id, globalDC, data_value);
				} else {
					PickToolAddToSelection(sel, current_seg, globalDC);
				}
			} else if (doselection || brushSize->Diameter() < 4) {
				if (!doselection) {
					BrushToolApplyPaint(segmentation_id, globalDC, data_value);
				} else {
					PickToolAddToSelection(sel, current_seg, globalDC);
				}
			}
		}
	} else {
		int fraction = dx - (dy >> 1);
		while (y0 != y1) {
			if (fraction >= 0) {
				x0 += stepx;
				fraction -= dy;
			}

			y0 += stepy;
			fraction += dx;

			// do the conversion ourselves; painting "skips" otherwise
			DataCoord globalDC;
			switch (mViewType) {
			case XY_VIEW:
				globalDC = DataCoord(x0, y0, second.z);
				break;
			case XZ_VIEW:
				globalDC = DataCoord(x0, second.y, y0);
				break;
			case YZ_VIEW:
				globalDC = DataCoord(second.x, y0, x0);
				break;
			}

			if(brushSize->Diameter() > 4 && (y1 == y0 || abs(y1 - y0) % (brushSize->Diameter() / 4) == 0)) {
				if (!doselection) {
					BrushToolApplyPaint(segmentation_id, globalDC, data_value);
				} else {
					PickToolAddToSelection(sel, current_seg, globalDC);
				}
			} else if (doselection || brushSize->Diameter() < 4) {
				if (!doselection){
					BrushToolApplyPaint(segmentation_id, globalDC, data_value);
				} else {
					PickToolAddToSelection(sel, current_seg, globalDC);
				}
			}
		}
	}
}

void OmLineDraw::PickToolAddToSelection( OmSegmentSelector & sel,
										 OmSegmentation & current_seg,
										 DataCoord globalDataClickPoint)
{
	const OmSegID segID = current_seg.GetVoxelValue(globalDataClickPoint);
	if (segID ) {
		sel.augmentSelectedSet( segID, true );
	}
}

void OmLineDraw::BrushToolApplyPaint(OmID segid, DataCoord gDC, OmSegID seg)
{
	boost::shared_ptr<OmBrushSize>& brushSize = state_->getBrushSize();

	DataCoord off = BrushToolToGDC(gDC);

	if (1 == brushSize->Diameter() ) {
		mEditedSegmentation = segid;
		mCurrentSegmentId = seg;

		if( gDC.x < 0 ||
			gDC.y < 0 ||
			gDC.z < 0){
			return;
		}
		mUpdatedDataCoords.insert(gDC);
		mDoRefresh = true;

	} else {
		const int savedDia = brushSize->Diameter();
		brushSize->SetDiameter(1);
		for (int i = 0; i < savedDia; i++) {
			for (int j = 0; j < savedDia; j++) {
				DataCoord myoff = off;
				float x = i - savedDia / 2.0;
				float y = j - savedDia / 2.0;
				if (sqrt(x * x + y * y) <= savedDia / 2.0) {
					myoff.x += i - savedDia / 2.0;
					myoff.y += j - savedDia / 2.0;
					BrushToolApplyPaint(segid,
										BrushToolToGDC(myoff),
										seg);
				}
			}
		}
		brushSize->SetDiameter(savedDia);
	}
}

void OmLineDraw::FillToolFill(OmID seg, DataCoord gCP, OmSegID fc,
							  OmSegID bc, int depth)
{
	OmSegmentation & segmentation = OmProject::GetSegmentation(seg);
	OmID segid = segmentation.GetVoxelValue(gCP);

	if (!segid) {
		return;
	}

	segid = segmentation.GetSegmentCache()->findRootID(segid);

	if (depth > 5000)
		return;
	++depth;

	if (segid == bc && segid != fc) {

		DataCoord off = BrushToolToGDC(gCP);

		off.x++;
		FillToolFill(seg, BrushToolToGDC(off), fc, bc, depth);
		off.y++;
		FillToolFill(seg, BrushToolToGDC(off), fc, bc, depth);
		off.x--;
		FillToolFill(seg, BrushToolToGDC(off), fc, bc, depth);
		off.x--;
		FillToolFill(seg, BrushToolToGDC(off), fc, bc, depth);
		off.y--;
		FillToolFill(seg, BrushToolToGDC(off), fc, bc, depth);
		off.y--;
		FillToolFill(seg, BrushToolToGDC(off), fc, bc, depth);
		off.x++;
		FillToolFill(seg, BrushToolToGDC(off), fc, bc, depth);
		off.x++;
		FillToolFill(seg, BrushToolToGDC(off), fc, bc, depth);
	}
}

void OmLineDraw::RemoveModifiedTiles()
{
	std::set<SpaceCoord> spaceCoordsToRemove;

	FOR_EACH(iter, mUpdatedDataCoords){
		spaceCoordsToRemove.insert(state_->computeTileSpaceCoord(*iter));
	}

	FOR_EACH(iter, spaceCoordsToRemove){
		OmTileCache::RemoveSpaceCoord(*iter);
	}
}

void OmLineDraw::myUpdate()
{
	if(!mDoRefresh){
		return;
	}

	if(mEditedSegmentation){
		OmActions::SetVoxels(mEditedSegmentation,
							 mUpdatedDataCoords,
							 mCurrentSegmentId);
		RemoveModifiedTiles();
	} else {
		state_->touchFreshnessAndRedraw2d();
	}
	mUpdatedDataCoords.clear();
	mEditedSegmentation = 0;
	mDoRefresh = false;
}

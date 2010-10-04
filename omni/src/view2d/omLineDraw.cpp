#include "viewGroup/omBrushSize.hpp"
#include "project/omProject.h"
#include "segment/actions/omSegmentEditor.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentSelector.h"
#include "tiles/cache/omTileCache.h"
#include "system/omStateManager.h"
#include "viewGroup/omViewGroupState.h"
#include "utility/dataWrappers.h"
#include "view2d/omLineDraw.hpp"
#include "view2d/omView2dState.hpp"
#include "volume/omSegmentation.h"
#include "volume/omVoxelSetValueAction.h"

OmLineDraw::OmLineDraw(boost::shared_ptr<OmView2dState> v2ds,
					   const ViewType vt)
	: state_(v2ds)
	, mViewType(vt)
{
	mDoRefresh = false;
	mCurrentSegmentId = 0;
	mEditedSegmentation = 0;

	mBrushToolMaxX = 0;
	mBrushToolMaxY = 0;
	mBrushToolMaxZ = 0;
	mBrushToolMinX = INT_MAX;
	mBrushToolMinY = INT_MAX;
	mBrushToolMinZ = INT_MAX;
}

void OmLineDraw::bresenhamLineDraw(const DataCoord & first,
								   const DataCoord & second,
								   bool doselection)
{
	//store current selection
	SegmentDataWrapper sdw = OmSegmentEditor::GetEditSelection();

	//return if not valid
	if (!sdw.isValid())
		return;

	const OmId segmentation_id = sdw.getSegmentationID();

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

	case SELECT_VOXEL_MODE:
		doselection = true;
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

	OmSegmentSelector sel(segmentation_id, this, "view2d_selector" );
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

			DataCoord globalDC =
				state_->makeViewTypeVector3(x0,
											y0,
											second.z);

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

			DataCoord globalDC =
				state_->makeViewTypeVector3(x0,
											y0,
											second.z);

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

void OmLineDraw::BrushToolApplyPaint(OmId segid, DataCoord gDC, OmSegID seg)
{
	boost::shared_ptr<OmBrushSize>& brushSize = state_->getBrushSize();

	DataCoord off = BrushToolToGDC(gDC);

	if (1 == brushSize->Diameter() ) {
		mEditedSegmentation = segid;
		mCurrentSegmentId = seg;
		mUpdatedDataCoords.insert(gDC);
		mDoRefresh = true;

		if (gDC.x > mBrushToolMaxX) {
			mBrushToolMaxX = gDC.x;
		}
		if (gDC.y > mBrushToolMaxY) {
			mBrushToolMaxY = gDC.y;
		}
		if (gDC.z > mBrushToolMaxZ) {
			mBrushToolMaxZ = gDC.z;
		}

		if (gDC.x < mBrushToolMinX) {
			mBrushToolMinX = gDC.x;
		}
		if (gDC.y < mBrushToolMinY) {
			mBrushToolMinY = gDC.y;
		}
		if (gDC.z < mBrushToolMinZ) {
			mBrushToolMinZ = gDC.z;
		}
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

void OmLineDraw::FillToolFill(OmId seg, DataCoord gCP, OmSegID fc,
							  OmSegID bc, int depth)
{
	OmSegmentation & segmentation = OmProject::GetSegmentation(seg);
	OmId segid = segmentation.GetVoxelValue(gCP);

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
		state_->getCache()->RemoveSpaceCoord(*iter);
	}
}

void OmLineDraw::myUpdate()
{
	if(mDoRefresh) {
		if(mEditedSegmentation){
			(new OmVoxelSetValueAction(mEditedSegmentation,
									   mUpdatedDataCoords,
									   mCurrentSegmentId))->Run();
			RemoveModifiedTiles();
		} else {
			state_->touchFreshnessAndRedraw();
		}
		mUpdatedDataCoords.clear();
		mEditedSegmentation = 0;
		mDoRefresh = false;
	}

	mBrushToolMaxX = 0;
	mBrushToolMaxY = 0;
	mBrushToolMaxZ = 0;
	mBrushToolMinX = INT_MAX;
	mBrushToolMinY = INT_MAX;
	mBrushToolMinZ = INT_MAX;
}

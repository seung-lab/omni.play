#ifndef OM_KEY_EVENTS_HPP
#define OM_KEY_EVENTS_HPP

#include "segment/omSegmentSelected.hpp"
#include "view2d/omScreenShotSaver.hpp"
#include "view2d/omView2d.h"
#include "view2d/omView2dState.hpp"
#include "view2d/omView2dZoom.hpp"
#include "viewGroup/omBrushSize.hpp"
#include "segment/omSegmentUtils.hpp"

#include <QKeyEvent>

class OmKeyEvents{
private:
	OmView2d *const v2d_;
	OmView2dState *const state_;

public:
	OmKeyEvents(OmView2d* v2d, OmView2dState* state)
 		: v2d_(v2d)
		, state_(state)
	{}

	bool Press(QKeyEvent* event)
	{
		switch (event->key()) {
		case Qt::Key_M:
			v2d_->GetScreenShotSaver()->toggleEmitMovie();
			break;
		case Qt::Key_P:
		{
			const bool shiftKey = event->modifiers() & Qt::ShiftModifier;
			if(shiftKey){
				state_->getBrushSize()->IncreaseSize();
			} else {
				state_->getBrushSize()->DecreaseSize();
			}
			v2d_->myUpdate();
		}
		break;

		case Qt::Key_L:
			state_->ToggleLevelLock();
			v2d_->myUpdate();
			break;
		case Qt::Key_F:
			// Toggle fill mode.
			v2d_->myUpdate();
			break;
		case Qt::Key_Escape:
			v2d_->resetWindow();
			break;
		case Qt::Key_Minus:
			v2d_->Zoom()->KeyboardZoomOut();
			break;
		case Qt::Key_Equal:
			v2d_->Zoom()->KeyboardZoomIn();
			break;
		case Qt::Key_Right:
		{
			const Vector2f current_pan = state_->ComputePanDistance();
			state_->SetPanDistance(current_pan.x + 5, current_pan.y);
		}
		break;
        	case Qt::Key_C:
        	{
                	SegmentationDataWrapper sdw(1);
                	OmSegmentUtils::CenterSegment(state_->getViewGroupState(), sdw);
        	}
                break;
		case Qt::Key_Left:
		{
			const Vector2f current_pan = state_->ComputePanDistance();
			state_->SetPanDistance(current_pan.x - 5, current_pan.y);
		}
		break;
		case Qt::Key_Up:
		{
			const Vector2i current_pan = state_->ComputePanDistance();
			state_->SetPanDistance(current_pan.x, current_pan.y + 5);
		}
		break;
		case Qt::Key_Down:
		{
			const Vector2f current_pan = state_->ComputePanDistance();
			state_->SetPanDistance(current_pan.x, current_pan.y - 5);
		}
		break;
		case Qt::Key_W:
		case Qt::Key_PageUp:
			state_->MoveUpStackCloserToViewer();
			OmEvents::ViewCenterChanged();
			break;
		case Qt::Key_S:
		case Qt::Key_PageDown:
			state_->MoveDownStackFartherFromViewer();
			OmEvents::ViewCenterChanged();
			break;
		case Qt::Key_Tab:
		{
			const bool control = event->modifiers() & Qt::ControlModifier;
			if(control){
				v2d_->ShowComplimentaryDock();
			}
		}
		break;
		default:
			return false;
		}

		return true; // we handled event
	}
};

#endif

#ifndef OM_SCREEN_PAINTER_HPP
#define OM_SCREEN_PAINTER_HPP

#include "utility/omTimer.h"
#include "view2d/omView2d.h"
#include "view2d/omView2dState.hpp"
#include "system/omEvents.h"
#include "view2d/omScreenShotSaver.hpp"
#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "view2d/omDisplayInfo.hpp"

#include <QPainter>

class OmScreenPainter{
public:
	OmScreenPainter(OmView2d* v2d,
					boost::shared_ptr<OmView2dState> state,
					boost::shared_ptr<OmScreenShotSaver> s)
		: v2d_(v2d)
		, state_(state)
		, screenShotSaver_(s)
		, viewType_(v2d->GetViewType())
		, shouldDisplayInfo_(OmPreferences::GetBoolean(om::PREF_VIEW2D_SHOW_INFO_BOOL))
	{}

	void FullRedraw2d() {
		elapsed_.restart();

		const QImage screenImage = v2d_->FullRedraw2d();
		paintScreen(screenImage);

		screenShotSaver_->saveTiles(screenImage);

		state_->SetViewSliceOnPan();

		if(!v2d_->IsDrawComplete()){
			OmEvents::Redraw2d();
		}
	}

private:
	OmView2d* v2d_;
	boost::shared_ptr<OmView2dState> state_;
	boost::shared_ptr<OmScreenShotSaver> screenShotSaver_;
	OmTimer elapsed_;
	const ViewType viewType_;
	const bool shouldDisplayInfo_;

	void paintScreen(const QImage& screenImage)
	{
		QPainter painter(v2d_);
		painter.drawImage(QPoint(0, 0), screenImage);

		QPen the_pen;
		the_pen.setColor(getPenColor());
		painter.setPen(the_pen);

		const Vector2i& mousePoint = state_->GetMousePoint();

		if(v2d_->amInFillMode()) {
			painter.drawRoundedRect(QRect(mousePoint.x,
										  mousePoint.y,
										  20,
										  20),
									5, 5);
		}else if(usingEditingTool()){
			const float zoomFactor = state_->getZoomScale();
			const int brushDiamater = zoomFactor *
				state_->getBrushSize()->Diameter();
			const double offset = 0.5 * brushDiamater;
			const double width = 1.0 * brushDiamater;
			painter.drawEllipse(QRectF
								(mousePoint.x - offset,
								 mousePoint.y - offset,
								 width,
								 width ));
		}

		if(v2d_->hasFocus()){
			the_pen.setWidth(5);
		}

		const Vector4i& vp = state_->getTotalViewport();
		painter.drawRect(vp.lowerLeftX,
						 vp.lowerLeftY,
						 vp.width - 1,
						 vp.height - 1);

		if(shouldDisplayInfo_){
			displayInformation(painter);
		}

		drawCursors(painter);
	}

	QColor getPenColor() const
	{
		switch(viewType_) {
		case XY_VIEW:
			return QColor(Qt::blue);
		case XZ_VIEW:
			return QColor(Qt::green);
		case YZ_VIEW:
			return QColor(Qt::red);
		default:
			throw OmArgException("invalid view type");
		}
	}

	void displayInformation(QPainter& painter)
	{
		const int xoffset = 10;
		const int yTopOfText = state_->getTotalViewport().height - 65;
		OmDisplayInfo di(painter, yTopOfText, xoffset);

		if(state_->IsLevelLocked()){
			di.paint("MIP Level Locked (Press L to unlock.)");
		}

		di.paint(state_->getMipLevel(), "Level");

		di.paint(state_->getZoomScale(), "zoomFactor", 2);

		di.paint(state_->getSliceDepth(), "Slice Depth");

		printTileCount(di);
		printTimingInfo(di);
	}

	void printTileCount(OmDisplayInfo& di)
	{
		const int tileCountIncomplete = v2d_->GetTileCountIncomplete();
		const int tileCount = v2d_->GetTileCount();

		if(tileCountIncomplete){
			di.paint(tileCountIncomplete, "tiles incomplete of",
					 tileCount, "tiles");
		}else{
			di.paint(tileCount, "tiles");
		}
	}

	void printTimingInfo(OmDisplayInfo& di)
	{
		const double timeMS = elapsed_.ms_elapsed();

		if(!state_->getScribbling()) {
			di.paint(timeMS, "ms", 1);
		}

		di.paint(1000.0 / timeMS, "fps", 0);
	}

	void drawCursors(QPainter& painter)
	{
		const Vector4i& vp = state_->getTotalViewport();
		const int fullHeight = vp.height;
		const int halfHeight = fullHeight/2;
		const int fullWidth = vp.width;
		const int halfWidth = fullWidth/2;
		const int a = 20;

		std::pair<QColor, QColor> colors = getCursorColors();

		painter.setPen(colors.first);
		painter.drawLine(halfWidth, 0, halfWidth, halfHeight - a);
		painter.drawLine(halfWidth, halfHeight + a, halfWidth, fullHeight);

		painter.setPen(colors.second);
		painter.drawLine(0, halfHeight, halfWidth - a, halfHeight);
		painter.drawLine(fullWidth, halfHeight, halfWidth + a, halfHeight);
	}

	std::pair<QColor, QColor> getCursorColors() const
	{
		switch (viewType_) {
		case XY_VIEW:
			return std::make_pair(Qt::red, Qt::green);
		case XZ_VIEW:
			return std::make_pair(Qt::red, Qt::blue);
		case YZ_VIEW:
			return std::make_pair(Qt::blue, Qt::green);
		default:
			throw OmArgException("invalid viewtype");
		}
	}

	bool usingEditingTool() const
	{
		switch(OmStateManager::GetToolMode()){
		case ADD_VOXEL_MODE:
		case SUBTRACT_VOXEL_MODE:
		case FILL_MODE:
			return true;
		default:
			return false;
		}
	}
};

#endif

#ifndef OM_BRUSH_OPP_LINE_HPP
#define OM_BRUSH_OPP_LINE_HPP

#include "view2d/brush/omBrushOppTypes.h"
#include "view2d/brush/omBrushOppUtils.hpp"

class OmBrushOppLine {
private:
    const boost::shared_ptr<OmBrushOppInfo> info_;
    const int brushDia_;
    const int depth_;
    const ViewType viewType_;

    std::vector<int> ptsInLineRadius1x_;
    std::vector<int> ptsInLineRadius1y_;

public:
    OmBrushOppLine(boost::shared_ptr<OmBrushOppInfo> info)
        : info_(info)
        , brushDia_(info_->brushDia)
        , depth_(info_->depth)
        , viewType_(info_->viewType)
    {}

    virtual ~OmBrushOppLine()
    {}

    boost::shared_ptr<om::pt3d_list_t>
    GetPts(const DataCoord& first, const DataCoord& second)
    {
        const Vector2i pt0 = OmView2dConverters::Get2PtsInPlane(first, info_->viewType);
        const Vector2i pt1 = OmView2dConverters::Get2PtsInPlane(second, info_->viewType);

        ptsInLineRadius1x_.reserve(100);
        ptsInLineRadius1y_.reserve(100);
        doBresenhamLineDraw(pt0.x, pt0.y, pt1.x, pt1.y);

        // we have list of pts in a line of radius one; grow out line width
        return getPtsInRadiusedLine();
    }

private:

    boost::shared_ptr<om::pt3d_list_t> getPtsInRadiusedLine()
    {
        boost::shared_ptr<om::pt3d_list_t> ret =
            boost::make_shared<om::pt3d_list_t>();

        om::pt3d_list_t* pts = ret.get();
        OmBrushOppInfo* info = info_.get();

        const int numPts = ptsInLineRadius1x_.size();

        for(int i = 0; i < numPts; ++i)
        {
            const Vector3i xyzCoord =
                OmView2dConverters::MakeViewTypeVector3<int>(ptsInLineRadius1x_[i],
                                                             ptsInLineRadius1y_[i],
                                                             depth_, viewType_);
            OmBrushOppUtils::GetPts(info, pts, xyzCoord, viewType_);
        }

        return ret;
    }

    inline void addPt(const int x, const int y)
    {
        ptsInLineRadius1x_.push_back(x);
        ptsInLineRadius1y_.push_back(y);
    }

    void doBresenhamLineDraw(int x0, int y0, int x1, int y1)
    {
        int dy = y1 - y0;
        int stepy = 1;
        if (dy < 0) {
            dy = -dy;
            stepy = -1;
        }

        int dx = x1 - x0;
        int stepx = 1;
        if (dx < 0) {
            dx = -dx;
            stepx = -1;
        }

        dy <<= 1;  // dy is now 2*dy
        dx <<= 1;  // dx is now 2*dx

        if (dx > dy) {
            int fraction = dy - (dx >> 1); // same as 2*dy - dx
            while (x0 != x1) {
                if (fraction >= 0) {
                    y0 += stepy;
                    fraction -= dx; // same as fraction -= 2*dx
                }
                x0 += stepx;
                fraction += dy; // same as fraction -= 2*dy

                if(brushDia_ > 4 &&
                   (x1 == x0 || abs(x1 - x0) % (brushDia_ / 4) == 0))
                {
                    addPt(x0, y0);

                } else if (brushDia_ < 4) {
                    addPt(x0, y0);
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

                if(brushDia_ > 4 &&
                   (y1 == y0 || abs(y1 - y0) % (brushDia_ / 4) == 0))
                {
                    addPt(x0, y0);

                } else if (brushDia_ < 4) {
                    addPt(x0, y0);
                }
            }
        }
    }
};

#endif

#pragma once

#include "common/common.h"
#include <QtGlobal>
#include <QDataStream>

class OmSegmentEdge {
public:
    OmSegmentEdge()
        : parentID(0)
        , childID(0)
        , threshold(-100)
        , valid(false)
    {}

    OmSegmentEdge(const om::common::SegID p, const om::common::SegID c, const double t)
        : parentID(p)
        , childID(c)
        , threshold(t)
        , valid(true)
    {}

    bool isValid() const {
        return valid;
    }

    bool operator==(const OmSegmentEdge & rhs) const
    {
        return (parentID == rhs.parentID &&
                childID == rhs.childID &&
                qFuzzyCompare(threshold, rhs.threshold) &&
                valid == rhs.valid );
    }

    om::common::SegID parentID;
    om::common::SegID childID;
    double threshold;
    bool valid;

    friend QDataStream &operator<<(QDataStream& out, const OmSegmentEdge& se)
	{
	    out << se.parentID;
	    out << se.childID;
	    out << se.threshold;

	    return out;
	};

	friend QDataStream &operator>>(QDataStream& in, OmSegmentEdge& se)
	{
	    in >> se.parentID;
	    in >> se.childID;
	    in >> se.threshold;

	    return in;
	};
};


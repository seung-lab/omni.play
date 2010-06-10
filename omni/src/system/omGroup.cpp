#include "system/omGroup.h"
#include "common/omDebug.h"

OmGroup::OmGroup(OmId id)
{
	mColor = GetRandomColor();
}

OmGroup::OmGroup(const OmSegIDsList & ids)
{
	mColor = GetRandomColor();
	AddIds(ids);
}

OmGroup::~OmGroup()
{
}

OmColor OmGroup::GetRandomColor()
{
	Vector3<float> color;
	OmColor colorInt;
    
        //initially random color
        do { 
                color.randomize();
        } while ((color.x * 255 > 255 && color.y * 255 > 255 && color.z * 255 > 255) &&
                 (color.x * 255 < 55 && color.y * 255 < 55 && color.z * 255 < 55));
    
        color.x /= 2;
        color.y /= 2;
        color.z /= 2;

        colorInt.red   = color.x * 255;
        colorInt.green = color.y * 255;
        colorInt.blue  = color.z * 255;

	return colorInt;
}

void OmGroup::AddIds(const OmSegIDsList & ids)
{
	// FIXME need to add not replace...
	debug("groups", "FIXME, replacing current ids instead of adding.\n");
	mIDs = ids;
}



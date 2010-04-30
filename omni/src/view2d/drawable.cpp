#include "drawable.h"

Drawable::Drawable(int x, int y, int tileLength, OmTileCoord tileCoord, float zoomFactor,
		   QExplicitlySharedDataPointer < OmTextureID > gotten_id)
:x(x), y(y), tileLength(tileLength), tileCoord(tileCoord), zoomFactor(zoomFactor), gotten_id(gotten_id)
{
	mGood = true;
}

Drawable::~Drawable ()
{
	//debug ("genone", "freeing?\n");
	gotten_id = QExplicitlySharedDataPointer < OmTextureID > ();
}

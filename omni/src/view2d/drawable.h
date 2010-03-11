#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "omTileCoord.h"
#include "omTextureID.h"

using boost::shared_ptr;

class Drawable {
public:
        Drawable (int x, int y, int tileLength, OmTileCoord tileCoord, float zoomFactor, 
		  shared_ptr<OmTextureID> gotten_id);

	~Drawable ();

        int x;
	int y;
	int tileLength;
	OmTileCoord tileCoord;
	float zoomFactor;
        shared_ptr<OmTextureID> gotten_id;

	bool mGood;
};

#endif
